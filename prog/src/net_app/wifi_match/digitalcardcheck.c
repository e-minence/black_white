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
#include "savedata/battlematch_savedata.h"
#include "savedata/rndmatch_savedata.h"

//  WIFI�o�g���}�b�`�̃��W���[��
#include "net_app/wifi_match/wifibattlematch_graphic.h"
#include "net_app/wifi_match/wifibattlematch_view.h"
#include "net_app/wifi_match/wifibattlematch_util.h"

//  �O�����J
#include "digitalcardcheck.h"

//-------------------------------------
///	�I�[�o�[���C
//=====================================

//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef PM_DEBUG
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
struct _DIGITALCARD_CHECK_WORK
{
	//���ʂŎg�����b�Z�[�W
	GFL_MSGDATA	          		*p_msg;

	//���ʂŎg���P��
	WORDSET				          	*p_word;

	//���ʏ��
	PLAYERINFO_WORK         	*p_playerinfo;

  //���X�g
  WBM_LIST_WORK             *p_list;

  //���C���V�[�P���X
  WBM_SEQ_WORK              *p_seq;

  //����
  DIGITALCARD_CHECK_PARAM   param;

  //�q�[�vID
  HEAPID                    heapID;
} ;

//=============================================================================
/**
 *					�v���g�^�C�v
*/
//=============================================================================
//-------------------------------------
///	WIFI���V�[�P���X�֐�
//=====================================
static void DC_SEQFUNC_Init( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_SignUp( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_Entry( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_Retire( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_ChangeDS( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_NoData( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_End( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	���W���[�����g���₷���܂Ƃ߂�����
//=====================================
//�v���C���[���
static void Util_PlayerInfo_Create( DIGITALCARD_CHECK_WORK *p_wk );
static void Util_PlayerInfo_Delete( DIGITALCARD_CHECK_WORK *p_wk );
static BOOL Util_PlayerInfo_MoveIn( DIGITALCARD_CHECK_WORK *p_wk );
static BOOL Util_PlayerInfo_MoveOut( DIGITALCARD_CHECK_WORK *p_wk );
static void Util_PlayerInfo_RenewalData( DIGITALCARD_CHECK_WORK *p_wk, PLAYERINFO_WIFI_UPDATE_TYPE type );
//�I����
typedef enum
{ 
  UTIL_LIST_TYPE_YESNO,
  UTIL_LIST_TYPE_RETURN,
  UTIL_LIST_TYPE_UNREGISTER,
}UTIL_LIST_TYPE;
static void Util_List_Create( DIGITALCARD_CHECK_WORK *p_wk, UTIL_LIST_TYPE type );
static void Util_List_Delete( DIGITALCARD_CHECK_WORK *p_wk );
static u32 Util_List_Main( DIGITALCARD_CHECK_WORK *p_wk );

//�e�L�X�g
static void Util_Text_Print( DIGITALCARD_CHECK_WORK *p_wk, u32 strID, WBM_TEXT_TYPE type );
static BOOL Util_Text_IsEnd( DIGITALCARD_CHECK_WORK *p_wk );
static void Util_Text_SetVisible( DIGITALCARD_CHECK_WORK *p_wk, BOOL is_visible );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �f�W�^���I��؉��  �J�n
 *
 *	@param	const DIGITALCARD_CHECK_PARAM *cp_param ����
 *	@param	heapID  �q�[�vID
 *
 *	@return �n���h��
 */
//-----------------------------------------------------------------------------
DIGITALCARD_CHECK_WORK *DIGITALCARD_CHECK_Init( const DIGITALCARD_CHECK_PARAM *cp_param, HEAPID heapID )
{ 
  DIGITALCARD_CHECK_WORK	  *p_wk;

  p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(DIGITALCARD_CHECK_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(DIGITALCARD_CHECK_WORK) );
  p_wk->param         = *cp_param;
  p_wk->heapID        = heapID;

  //���\�[�X�ǉ�
  WIFIBATTLEMATCH_VIEW_LoadScreen( p_wk->param.p_view, WIFIBATTLEMATCH_VIEW_RES_MODE_DIGITALCARD, heapID );

  //���ʃ��W���[���쐬
  p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_match_dat, heapID );
	p_wk->p_word	= WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_COUNTRY_BUFLEN, heapID );

  //���W���[���쐬
  p_wk->p_seq   = WBM_SEQ_Init( p_wk, DC_SEQFUNC_Init, heapID );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�W�^���I��؉��  �I��
 *
 *	@param	DIGITALCARD_CHECK_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DIGITALCARD_CHECK_Exit( DIGITALCARD_CHECK_WORK *p_wk )
{ 
  //���W���[���̔j��
  WBM_SEQ_Exit( p_wk->p_seq );
  Util_PlayerInfo_Delete( p_wk );

	//���ʃ��W���[���̔j��
	WORDSET_Delete( p_wk->p_word );
	GFL_MSG_Delete( p_wk->p_msg );

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�W�^���I��؉��  ����
 *
 *	@param	DIGITALCARD_CHECK_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void DIGITALCARD_CHECK_Main( DIGITALCARD_CHECK_WORK *p_wk )
{ 
  //���C���V�[�P���X
  WBM_SEQ_Main( p_wk->p_seq );

  //�����\��
  if( p_wk->p_playerinfo )
  { 
    PLAYERINFO_PrintMain( p_wk->p_playerinfo, p_wk->param.p_que );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�W�^���I��؉��  �I���`�F�b�N
 *
 *	@param	const DIGITALCARD_CHECK_WORK *cp_wk w�A�[�N
 *
 *	@return TRUE�I��  FALSE������
 */
//-----------------------------------------------------------------------------
BOOL DIGITALCARD_CHECK_IsEnd( const DIGITALCARD_CHECK_WORK *cp_wk )
{ 
  return WBM_SEQ_IsEnd( cp_wk->p_seq );
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
  DIGITALCARD_CHECK_WORK	  *p_wk	    = p_wk_adrs;
  SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
  REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
  REGULATION_CARDDATA *p_reg    = RegulationSaveData_GetRegulationCard( p_reg_sv, p_wk->param.type );
  const u32 cup_no      = Regulation_GetCardParam( p_reg, REGULATION_CARD_CUPNO );
  const u32 cup_status  = Regulation_GetCardParam( p_reg, REGULATION_CARD_STATUS );

  NAGI_Printf( "card NO%d status%d\n", cup_no, cup_status );

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
  else if( cup_no > 0 && cup_status == DREAM_WORLD_MATCHUP_CHANGE_DS )
  { 
    //DS�������ւ���
    WBM_SEQ_SetNext( p_seqwk, DC_SEQFUNC_ChangeDS );
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
    SEQ_WAIT_MOVEOUT_PLAYERINFO,
    SEQ_PROC_END,
  };
  DIGITALCARD_CHECK_WORK	  *p_wk	    = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_PLAYERINFO:
    Util_PlayerInfo_Create( p_wk );
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNREGISTER );
    *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    if( Util_PlayerInfo_MoveIn( p_wk ) )
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
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_00, WBM_TEXT_TYPE_STREAM );
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
          *p_seq  = SEQ_WAIT_MOVEOUT_PLAYERINFO;
        }
      }
    }
    break;

  case SEQ_WAIT_MOVEOUT_PLAYERINFO:
    if( Util_PlayerInfo_MoveOut( p_wk ) )
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

    SEQ_WAIT_MOVEOUT_PLAYERINFO,
    SEQ_PROC_END,
    SEQ_WAIT_MSG,
  };
  DIGITALCARD_CHECK_WORK	  *p_wk	    = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_PLAYERINFO:
    Util_PlayerInfo_Create( p_wk );
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_LOCK );
    *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    if( Util_PlayerInfo_MoveIn( p_wk ) )
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
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_01, WBM_TEXT_TYPE_STREAM  );
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
          *p_seq  = SEQ_WAIT_MOVEOUT_PLAYERINFO;
        }
      }
    }
    break;

  case SEQ_START_MSG_CONFIRM1:
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_02, WBM_TEXT_TYPE_STREAM  );
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
          *p_seq  = SEQ_WAIT_MOVEOUT_PLAYERINFO;
        }
      }
    }
    break;
  case SEQ_START_MSG_CONFIRM2:
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_03, WBM_TEXT_TYPE_STREAM  );
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
          *p_seq  = SEQ_START_MSG_UNREGISTER;
        }
        else if( select == 1 )  //������
        { 
          *p_seq  = SEQ_WAIT_MOVEOUT_PLAYERINFO;
        }
      }
    }
    break;    

  case SEQ_START_MSG_UNREGISTER:
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_04, WBM_TEXT_TYPE_WAIT );
    *p_seq  = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_RETIRE );
    break;

  case SEQ_RETIRE:
    { 
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
      REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
      REGULATION_CARDDATA *p_reg    = RegulationSaveData_GetRegulationCard( p_reg_sv, p_wk->param.type ); 
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      BATTLE_BOX_LOCK_BIT bit = (p_wk->param.type == REGULATION_CARD_TYPE_WIFI) ? BATTLE_BOX_LOCK_BIT_WIFI:BATTLE_BOX_LOCK_BIT_LIVE;

      BATTLE_BOX_SAVE_OffLockFlg( p_bbox_save, bit );
      Regulation_SetCardParam( p_reg, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_RETIRE );
      *p_seq  = SEQ_START_SAVE;
    }
    break;
    
  case SEQ_START_SAVE:
    { 
      GAMEDATA_SaveAsyncStart( p_wk->param.p_gamedata );
      *p_seq  = SEQ_WAIT_SAVE;
    }
    break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT ret;
      ret = GAMEDATA_SaveAsyncMain( p_wk->param.p_gamedata );
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
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_05, WBM_TEXT_TYPE_STREAM  );
    *p_seq  = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_WAIT_MOVEOUT_PLAYERINFO );
    break;

  case SEQ_WAIT_MOVEOUT_PLAYERINFO:
    if( Util_PlayerInfo_MoveOut( p_wk ) )
    { 
      *p_seq  = SEQ_PROC_END;
    }
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
    SEQ_WAIT_MOVEOUT_PLAYERINFO,
    SEQ_PROC_END,
  };
  DIGITALCARD_CHECK_WORK	  *p_wk	    = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_PLAYERINFO:
    Util_PlayerInfo_Create( p_wk );
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK );
    *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    if( Util_PlayerInfo_MoveIn( p_wk ) )
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
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_06, WBM_TEXT_TYPE_STREAM  );
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
          *p_seq  = SEQ_WAIT_MOVEOUT_PLAYERINFO;
        }
      }
    }
    break;

  case SEQ_WAIT_MOVEOUT_PLAYERINFO:
    if( Util_PlayerInfo_MoveOut( p_wk ) )
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
    SEQ_WAIT_MOVEOUT_PLAYERINFO,
    SEQ_PROC_END,
  };
  DIGITALCARD_CHECK_WORK	  *p_wk	    = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_PLAYERINFO:
    Util_PlayerInfo_Create( p_wk );
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK );
    *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    if( Util_PlayerInfo_MoveIn( p_wk ) )
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
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_07, WBM_TEXT_TYPE_STREAM  );
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
          *p_seq  = SEQ_WAIT_MOVEOUT_PLAYERINFO;
        }
      }
    }
    break;

  case SEQ_WAIT_MOVEOUT_PLAYERINFO:
    if( Util_PlayerInfo_MoveOut( p_wk ) )
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
 *	@brief  �f�W�^���I��؃`�F�b�N  DS�������ւ���
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void DC_SEQFUNC_ChangeDS( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
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
    SEQ_WAIT_MOVEOUT_PLAYERINFO,
    SEQ_PROC_END,
  };
  DIGITALCARD_CHECK_WORK	  *p_wk	    = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_PLAYERINFO:
    Util_PlayerInfo_Create( p_wk );
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK );
    *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    if( Util_PlayerInfo_MoveIn( p_wk ) )
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
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_09, WBM_TEXT_TYPE_STREAM  );
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
          *p_seq  = SEQ_WAIT_MOVEOUT_PLAYERINFO;
        }
      }
    }
    break;

  case SEQ_WAIT_MOVEOUT_PLAYERINFO:
    if( Util_PlayerInfo_MoveOut( p_wk ) )
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
  DIGITALCARD_CHECK_WORK	  *p_wk	    = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_MSG_NODATA:
    Util_Text_SetVisible( p_wk, TRUE );
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_08, WBM_TEXT_TYPE_STREAM  );
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
  DIGITALCARD_CHECK_WORK	  *p_wk	    = p_wk_adrs;
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
 *	@param	DIGITALCARD_CHECK_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_Create( DIGITALCARD_CHECK_WORK *p_wk )
{ 
  if( p_wk->p_playerinfo == NULL )
  {
    SAVE_CONTROL_WORK   *p_sv = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
    REGULATION_SAVEDATA *p_reg_sv  = SaveData_GetRegulationSaveData(GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata ));
    REGULATION_CARDDATA *p_reg_card  = RegulationSaveData_GetRegulationCard(p_reg_sv, p_wk->param.type );
    REGULATION_VIEWDATA *p_reg_view  = RegulationSaveData_GetRegulationView( p_reg_sv, p_wk->param.type );
    REGULATION          *p_reg  = RegulationData_GetRegulation( p_reg_card );
    BATTLEMATCH_DATA    *p_btlmatch_sv = SaveData_GetBattleMatch( p_sv );
    MYSTATUS            *p_my       = GAMEDATA_GetMyStatus( p_wk->param.p_gamedata); 
    GFL_CLUNIT	        *p_unit     = WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->param.p_graphic );

    if( p_wk->param.type == REGULATION_CARD_TYPE_WIFI )
    {
      const RNDMATCH_DATA *cp_match_save;
      PLAYERINFO_WIFICUP_DATA info_setup;
      GFL_STD_MemClear( &info_setup, sizeof(PLAYERINFO_WIFICUP_DATA) );
      GFL_STD_MemCopy( Regulation_GetCardCupNamePointer( p_reg_card ), info_setup.cup_name, 2*(WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE) );

      cp_match_save  = BATTLEMATCH_GetRndMatchConst( p_btlmatch_sv );
      info_setup.rate = RNDMATCH_GetParam( cp_match_save, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_RATE );
      info_setup.btl_cnt = RNDMATCH_GetParam( cp_match_save, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_WIN ) + RNDMATCH_GetParam( cp_match_save, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_LOSE );

      info_setup.trainerID  = MyStatus_GetTrainerView( p_my );

      info_setup.start_date = GFDATE_Set( 
          Regulation_GetCardParam( p_reg_card, REGULATION_CARD_START_YEAR ),
          Regulation_GetCardParam( p_reg_card, REGULATION_CARD_START_MONTH ), 
          Regulation_GetCardParam( p_reg_card, REGULATION_CARD_START_DAY ),
          0);

      info_setup.end_date = GFDATE_Set( 
          Regulation_GetCardParam( p_reg_card, REGULATION_CARD_END_YEAR ),
          Regulation_GetCardParam( p_reg_card, REGULATION_CARD_END_MONTH ), 
          Regulation_GetCardParam( p_reg_card, REGULATION_CARD_END_DAY ),
          0);

      p_wk->p_playerinfo	= PLAYERINFO_WIFI_Init( &info_setup, FALSE, p_my, p_unit, p_wk->param.p_view, p_wk->param.p_font, p_wk->param.p_que, p_wk->p_msg, p_wk->p_word, p_reg_view, TRUE, p_wk->heapID );
    }
    else
    { 
      const LIVEMATCH_DATA *p_livematch;
      PLAYERINFO_LIVECUP_DATA info_setup;
      GFL_STD_MemClear( &info_setup, sizeof(PLAYERINFO_LIVECUP_DATA) );
      GFL_STD_MemCopy( Regulation_GetCardCupNamePointer( p_reg_card ), info_setup.cup_name, 2*(WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE) );

      p_livematch = BATTLEMATCH_GetLiveMatch( p_btlmatch_sv );
      info_setup.win_cnt  = LIVEMATCH_DATA_GetMyParam( p_livematch, LIVEMATCH_MYDATA_PARAM_WIN );
      info_setup.lose_cnt = LIVEMATCH_DATA_GetMyParam( p_livematch, LIVEMATCH_MYDATA_PARAM_LOSE );
      info_setup.btl_cnt  = LIVEMATCH_DATA_GetMyParam( p_livematch, LIVEMATCH_MYDATA_PARAM_BTLCNT );
      info_setup.btl_max  = Regulation_GetParam( p_reg, REGULATION_BTLCOUNT );

      info_setup.trainerID  = MyStatus_GetTrainerView( p_my );

      info_setup.start_date = GFDATE_Set( 
          Regulation_GetCardParam( p_reg_card, REGULATION_CARD_START_YEAR ),
          Regulation_GetCardParam( p_reg_card, REGULATION_CARD_START_MONTH ), 
          Regulation_GetCardParam( p_reg_card, REGULATION_CARD_START_DAY ),
          0);

      info_setup.end_date = GFDATE_Set( 
          Regulation_GetCardParam( p_reg_card, REGULATION_CARD_END_YEAR ),
          Regulation_GetCardParam( p_reg_card, REGULATION_CARD_END_MONTH ), 
          Regulation_GetCardParam( p_reg_card, REGULATION_CARD_END_DAY ),
          0);

      p_wk->p_playerinfo	= PLAYERINFO_LIVE_Init( &info_setup, p_my, p_unit, p_wk->param.p_view, p_wk->param.p_font, p_wk->param.p_que, p_wk->p_msg, p_wk->p_word, p_reg_view, TRUE, p_wk->heapID );
    }


    Util_Text_SetVisible( p_wk, FALSE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̏���j��
 *
 *	@param	DIGITALCARD_CHECK_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_Delete( DIGITALCARD_CHECK_WORK *p_wk )
{ 
  if( p_wk->p_playerinfo )
  { 
    if( p_wk->param.type == REGULATION_CARD_TYPE_WIFI )
    { 
      PLAYERINFO_WIFI_Exit( p_wk->p_playerinfo );
    }
    else
    { 
      PLAYERINFO_LIVE_Exit( p_wk->p_playerinfo );
    }
    p_wk->p_playerinfo  = NULL;
  }

  GFL_BG_ClearScreen( PLAYERINFO_BG_FRAME_MAIN );
  GFL_BG_LoadScreenReq( PLAYERINFO_BG_FRAME_MAIN );

}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h���X���C�h�C��
 *
 *	@param	DIGITALCARD_CHECK_WORK *p_wk  ���[�N
 *
 *	@return TRUE�Ŋ���  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL Util_PlayerInfo_MoveIn( DIGITALCARD_CHECK_WORK *p_wk )
{ 
  return PLAYERINFO_MoveMain( p_wk->p_playerinfo );;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h���X���C�h�A�E�g
 *
 *	@param	DIGITALCARD_CHECK_WORK *p_wk  ���[�N
 *
 *	@return TRUE�Ŋ���  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL Util_PlayerInfo_MoveOut( DIGITALCARD_CHECK_WORK *p_wk )
{ 
  return PLAYERINFO_MoveOutMain( p_wk->p_playerinfo );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h�̃f�[�^���X�V
 *
 *	@param	DIGITALCARD_CHECK_WORK *p_wk
 *	@param	type 
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_RenewalData( DIGITALCARD_CHECK_WORK *p_wk, PLAYERINFO_WIFI_UPDATE_TYPE type )
{ 
  if( p_wk->param.type == REGULATION_CARD_TYPE_WIFI )
  { 
    PLAYERINFO_WIFI_RenewalData( p_wk->p_playerinfo, type, p_wk->p_msg, p_wk->param.p_que, p_wk->param.p_font, p_wk->heapID );
  }
  else
  { 
    PLAYERINFO_LIVE_RenewalData( p_wk->p_playerinfo, type, p_wk->p_msg, p_wk->param.p_que, p_wk->param.p_font, p_wk->heapID );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g������
 *
 *	@param	DIGITALCARD_CHECK_WORK *p_wk  ���[�N
 *	@param	type                            ���X�g�̎��
 */
//-----------------------------------------------------------------------------
static void Util_List_Create( DIGITALCARD_CHECK_WORK *p_wk, UTIL_LIST_TYPE type )
{ 
  enum
  { 
    POS_RIGHT_DOWN,
    POS_RIGHT_DOWN_LONG,
  } pos;
  u32 x,y,w,h;

  if( p_wk->p_list == NULL )
  { 
    WBM_LIST_SETUP  setup;
    GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
    setup.p_msg   = p_wk->p_msg;
    setup.p_font  = p_wk->param.p_font;
    setup.p_que   = p_wk->param.p_que;
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
      setup.is_cancel = TRUE;
      setup.cancel_idx  = 1;
      pos = POS_RIGHT_DOWN;
      break;
    case UTIL_LIST_TYPE_RETURN:
      setup.strID[0]= WIFIMATCH_DPC_SELECT_00;
      setup.list_max= 1;
      setup.is_cancel = TRUE;
      setup.cancel_idx  = 0;
      pos = POS_RIGHT_DOWN;
      break;
    case UTIL_LIST_TYPE_UNREGISTER:
      setup.strID[0]= WIFIMATCH_DPC_SELECT_01;
      setup.strID[1]= WIFIMATCH_DPC_SELECT_00;
      setup.list_max= 2;
      setup.is_cancel = TRUE;
      setup.cancel_idx  = 1;
      pos = POS_RIGHT_DOWN_LONG;
      break;
    }

    switch( pos )
    { 
    case POS_RIGHT_DOWN:
      w  = 12;
      h  = setup.list_max * 2;
      x  = 32 - w - 1; //1�̓t���[����
      y  = 24 - h - 1 - 6; //�P�͎����̃t���[������6�̓e�L�X�g��
      break;

    case POS_RIGHT_DOWN_LONG:
      w  = 15;
      h  = setup.list_max * 2;
      x  = 32 - w - 1; //1�̓t���[����
      y  = 24 - h - 1 - 6; //�P�͎����̃t���[������6�̓e�L�X�g��
    break;
    }

    p_wk->p_list  = WBM_LIST_InitEx( &setup, x, y, w, h, p_wk->heapID );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�j��
 *
 *	@param	DIGITALCARD_CHECK_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_List_Delete( DIGITALCARD_CHECK_WORK *p_wk )
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
 *	@param	DIGITALCARD_CHECK_WORK *p_wk ���[�N
 *
 *	@return �I����������
 */
//-----------------------------------------------------------------------------
static u32 Util_List_Main( DIGITALCARD_CHECK_WORK *p_wk )
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
 *	@param	DIGITALCARD_CHECK_WORK *p_wk ���[�N
 *	@param	strID                       ����ID
 */
//-----------------------------------------------------------------------------
static void Util_Text_Print( DIGITALCARD_CHECK_WORK *p_wk, u32 strID, WBM_TEXT_TYPE type )
{ 
  WBM_TEXT_Print( p_wk->param.p_text, p_wk->p_msg, strID, type );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�̕����o�͂��I��������ǂ���
 *
 *	@param	DIGITALCARD_CHECK_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ŕ`��I��  FALSE�ŕ`�撆
 */
//-----------------------------------------------------------------------------
static BOOL Util_Text_IsEnd( DIGITALCARD_CHECK_WORK *p_wk )
{ 
  return WBM_TEXT_IsEnd( p_wk->param.p_text );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�̕\���ݒ�
 *
 *	@param	DIGITALCARD_CHECK_WORK *p_wk ���[�N
 *	@param	is_visible                  TRUE�ŕ\��FALSE�Ŕ�\��
 */
//-----------------------------------------------------------------------------
static void Util_Text_SetVisible( DIGITALCARD_CHECK_WORK *p_wk, BOOL is_visible )
{ 
  GFL_BG_SetVisible( BG_FRAME_M_TEXT, is_visible );
}
