//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		livebattlematch_flow.c
 *	@brief  ���C�u���t���[
 *	@author	Toru=Nagihashi
 *	@date		2010.03.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID
#include "poke_tool/pokeparty.h"
#include "gamesystem/btl_setup.h"
#include "print/str_tool.h"
#include "poke_tool/poke_regulation.h"
#include "sound/pm_sndsys.h"
#include "system/net_err.h"
#include "battle/btl_net.h" //BTL_NET_SERVER_VERSION

//�v���Z�X


//�A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "msg/msg_live_match.h"
#include "font/font.naix"

//�Z�[�u�f�[�^
#include "savedata/my_pms_data.h"
#include "savedata/battlematch_savedata.h"
#include "savedata/livematch_savedata.h"

//�l�b�g
#include "net/network_define.h"
#include "net/dreamworld_netdata.h"

//WIFIBATTLEMATCH�̃��W���[��
#include "wifibattlematch_util.h"
#include "wifibattlematch_graphic.h"
#include "wifibattlematch_view.h"
#include "livebattlematch_irc.h"

//�O�����J
#include "livebattlematch_flow.h"

FS_EXTERN_OVERLAY( ui_common );

//-------------------------------------
///	DEBUG
//=====================================
#ifdef PM_DEBUG
#define DEBUG_REGULATIONCRC_PASS  //���M�����[�V������CRC�`�F�b�N��ʉ߂���
#define DEBUG_REGULATION_RECVCHECK_PASS  //���M�����[�V�����̎�M�`�F�b�N��ʉ߂���
#define DEBUG_REGULATION_RECV_A_PASS  //���M�����[�V�����̎�M��A�{�^���Ői��
#endif //PM_DEBUG

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�V���N
//=====================================
#define ENEMYDATA_WAIT_SYNC    (180)
#define MATCHING_MSG_WAIT_SYNC (120)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���C�u���[�N
//=====================================
struct _LIVEBATTLEMATCH_FLOW_WORK
{ 
  //�V�[�P���X���䃂�W���[��
  WBM_SEQ_WORK                  *p_seq;

  //�I�����\�����W���[��
  WBM_LIST_WORK                 *p_list;
  u32                           list_type;

  //�l�b�g���W���[��
  LIVEBATTLEMATCH_IRC_WORK      *p_irc;

	//���ʏ��
	PLAYERINFO_WORK             	*p_playerinfo;

	//�ΐ�ҏ��
	MATCHINFO_WORK              	*p_matchinfo;

  //�o�g���{�b�N�X
  WBM_BTLBOX_WORK               *p_btlbox;

  //�ҋ@�A�C�R��
  WBM_WAITICON_WORK             *p_wait;

  //���C�u���p���b�Z�[�W
  GFL_MSGDATA                   *p_msg;

  //�I��ؗp���b�Z�[�W(wifi_match�Ƌ��p)
  GFL_MSGDATA                   *p_wifi_msg;

  //�|�P�����̃��x���E���ʃ}�[�N�\���݂̂Ŏg�p���Ă���t�H���g
	GFL_FONT			                *p_small_font; 

  //�P��o�^
  WORDSET                       *p_word;

  //�|�P�p�[�e�B�e���|����
  POKEPARTY                     *p_party;

  //���C�u�}�b�`�Z�[�u�f�[�^
  LIVEMATCH_DATA                *p_livematch;

  //�f�W�^���I���  �Z�[�u�f�[�^
  REGULATION_CARDDATA           *p_regulation;

  //�f�W�^���I��؁@��M�o�b�t�@
  REGULATION_CARDDATA           regulation_temp;

  //�߂�l
  LIVEBATTLEMATCH_FLOW_RET      retcode;

  //����
  LIVEBATTLEMATCH_FLOW_PARAM    param;

  //�q�[�vID
  HEAPID                        heapID;

  //�ėp�J�E���^
  u32                           cnt;

  //�ӂ肩����p�ΐ�^�悪���邩
  BOOL                          is_rec;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�V�[�P���X�֐�
//=====================================
static void SEQFUNC_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_End( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

static void SEQFUNC_RecvCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_Register( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_StartCup( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_BtlAfter( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_RecAfter( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//-------------------------------------
///	�֗��֐�
//=====================================
//���X�g
typedef enum
{ 
  LVM_MENU_TYPE_YESNO,
  LVM_MENU_TYPE_MENU,
  LVM_MENU_TYPE_END,
}LVM_MENU_TYPE;
static void UTIL_LIST_Create( LIVEBATTLEMATCH_FLOW_WORK *p_wk, LVM_MENU_TYPE type );
static void UTIL_LIST_Delete( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
static u32 UTIL_LIST_Main( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
//�e�L�X�g
static void UTIL_TEXT_Print( LIVEBATTLEMATCH_FLOW_WORK *p_wk, u32 strID );
static BOOL UTIL_TEXT_IsEnd( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
//�t���[�I��
static void UTIL_FLOW_End( LIVEBATTLEMATCH_FLOW_WORK *p_wk, LIVEBATTLEMATCH_FLOW_RET ret );
//�����̃J�[�h�쐬
static void UTIL_PLAYERINFO_Create( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
static void UTIL_PLAYERINFO_Delete( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
static BOOL UTIL_PLAYERINFO_Move( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
static void UTIL_PLAYERINFO_RenewalData( LIVEBATTLEMATCH_FLOW_WORK *p_wk, PLAYERINFO_WIFI_UPDATE_TYPE type );
//�ΐ푊��̃J�[�h�쐬
static void UTIL_MATCHINFO_Create( LIVEBATTLEMATCH_FLOW_WORK *p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_enemy_data );
static void UTIL_MATCHINFO_Delete( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
static BOOL UTIL_MATCHINFO_Main( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
//�o�g���{�b�N�X
static void UTIL_BTLBOX_Create( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
static void UTIL_BTLBOX_Delete( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
static BOOL UTIL_BTLBOX_MoveIn( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
static BOOL UTIL_BTLBOX_MoveOut( LIVEBATTLEMATCH_FLOW_WORK *p_wk );
//�|�P�p�[�e�B��ݒ�
static void UTIL_DATA_GetBtlBoxParty( LIVEBATTLEMATCH_FLOW_WORK *p_wk, POKEPARTY *p_dst );
//�����̃f�[�^�쐬
static void UTIL_DATA_SetupMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, LIVEBATTLEMATCH_FLOW_WORK *p_wk );
//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//=============================================================================
/**
 *					�O�����J
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���C�u���t���[  ����
 *
 *	@param	const LIVEBATTLEMATCH_FLOW_PARAM *cp_param  ����
 *	@param	heapID  �q�[�vID 
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
LIVEBATTLEMATCH_FLOW_WORK *LIVEBATTLEMATCH_FLOW_Init( const LIVEBATTLEMATCH_FLOW_PARAM *cp_param, HEAPID heapID )
{ 
  LIVEBATTLEMATCH_FLOW_WORK *p_wk;

  //WIFIBATTLEMATCH_VIEW��PLAYERINFO_LIVE_Init�̒��Ŏg�p���邽��
  GFL_OVERLAY_Load(  FS_OVERLAY_ID( ui_common ) );

  //���[�N�쐬
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(LIVEBATTLEMATCH_FLOW_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(LIVEBATTLEMATCH_FLOW_WORK) );
  p_wk->param   = *cp_param;
  p_wk->heapID  = heapID;
  p_wk->is_rec  = BattleRec_DataSetCheck();

  //���\�[�X�ǉ�
  WIFIBATTLEMATCH_VIEW_LoadScreen( p_wk->param.p_view, WIFIBATTLEMATCH_VIEW_RES_MODE_LIVE, heapID );

  //���ʃ��W���[���쐬
  p_wk->p_word  = WORDSET_Create( heapID );
  p_wk->p_msg       = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_live_match_dat, heapID );
  p_wk->p_wifi_msg  = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_match_dat, heapID );

  //���ʃt�H���g�쐬
	p_wk->p_small_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_num_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, heapID );

  //���W���[���쐬
  p_wk->p_seq = WBM_SEQ_Init( p_wk, SEQFUNC_Start, heapID );
  p_wk->p_irc = LIVEBATTLEMATCH_IRC_Init( p_wk->param.p_gamedata, heapID );

  //�ҋ@�A�C�R��
  { 
    GFL_CLUNIT  *p_unit;
    p_unit  = WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->param.p_graphic );
    p_wk->p_wait  = WBM_WAITICON_Init( p_unit, p_wk->param.p_view, heapID );
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
  }

  //LIVE���p���M�����[�V�����Z�[�u�A�h���X
  { 
    SAVE_CONTROL_WORK   *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
    REGULATION_SAVEDATA *p_reg_sv   = SaveData_GetRegulationSaveData( p_sv_ctrl );
    p_wk->p_regulation  = RegulationSaveData_GetRegulationCard( p_reg_sv, REGULATION_CARD_TYPE_LIVE );
  }

  //���C�u�}�b�`�p�Z�[�u�f�[�^
  {
    SAVE_CONTROL_WORK   *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
    BATTLEMATCH_DATA    *p_btlmatch_sv  = SaveData_GetBattleMatch( p_sv_ctrl );
    p_wk->p_livematch = BATTLEMATCH_GetLiveMatch( p_btlmatch_sv );
  }

  //�|�P�p�[�e�B�e���|����
  p_wk->p_party = PokeParty_AllocPartyWork( heapID );


  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���C�u���t���[  �j��
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void LIVEBATTLEMATCH_FLOW_Exit( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{ 
  GFL_HEAP_FreeMemory( p_wk->p_party );

  //���W���[���j��
  UTIL_PLAYERINFO_Delete( p_wk );
  UTIL_MATCHINFO_Delete( p_wk );
  WBM_WAITICON_Exit( p_wk->p_wait );
  LIVEBATTLEMATCH_IRC_Exit( p_wk->p_irc );
  WBM_SEQ_Exit( p_wk->p_seq);

  //���ʃ��W���[���j��
  GFL_FONT_Delete( p_wk->p_small_font );
  GFL_MSG_Delete( p_wk->p_wifi_msg );
  GFL_MSG_Delete( p_wk->p_msg );
  WORDSET_Delete( p_wk->p_word );

  GFL_OVERLAY_Unload(  FS_OVERLAY_ID( ui_common ) );

  //���[�N�j��
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���C�u���t���[  ���C������
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void LIVEBATTLEMATCH_FLOW_Main( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{
  //�V�[�P���X���C��
  WBM_SEQ_Main( p_wk->p_seq );

  //�l�b�g���C��
  LIVEBATTLEMATCH_IRC_Main( p_wk->p_irc );

  //�ҋ@�A�C�R��
  WBM_WAITICON_Main( p_wk->p_wait );


  //�����\��
  if( p_wk->p_playerinfo )
  { 
    PLAYERINFO_PrintMain( p_wk->p_playerinfo, p_wk->param.p_que );
  }
  if( p_wk->p_matchinfo )
  { 
    MATCHINFO_PrintMain( p_wk->p_matchinfo, p_wk->param.p_que );
  }
  if( p_wk->p_btlbox )
  { 
    WBM_BTLBOX_PrintMain( p_wk->p_btlbox, p_wk->param.p_que );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C�u���t���[  �I�����m
 *
 *	@param	const LIVEBATTLEMATCH_FLOW_WORK *cp_wk  ���[�N
 *
 *	@return �I���R�[�h�i�ڍׂ͗񋓌^���Q�Ɓj
 */
//-----------------------------------------------------------------------------
LIVEBATTLEMATCH_FLOW_RET LIVEBATTLEMATCH_FLOW_IsEnd( const LIVEBATTLEMATCH_FLOW_WORK *cp_wk )
{ 
  if( WBM_SEQ_IsEnd( cp_wk->p_seq )  )
  { 
    return cp_wk->retcode;
  }
  return LIVEBATTLEMATCH_FLOW_RET_CONTINUE;
}

//=============================================================================
/**
 *  �V�[�P���X�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�J�n
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;

  switch( p_wk->param.mode )
  { 
  case LIVEBATTLEMATCH_FLOW_MODE_START:  //�ŏ�����J�n
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_RecvCard );
    break;
  case LIVEBATTLEMATCH_FLOW_MODE_MENU:    //���C�����j���[����J�n
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_StartCup );
    break;
  case LIVEBATTLEMATCH_FLOW_MODE_BTL:    //�o�g���ォ��J�n
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_BtlAfter );
    break;
  case LIVEBATTLEMATCH_FLOW_MODE_REC:    //�^��ォ��J�n
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_RecAfter );
    break;
  default:
    GF_ASSERT(0);
  }
  
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I��
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_seqwk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  //�I��
  WBM_SEQ_End( p_seqwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I��؎�M�V�[�P���X
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_RecvCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK_CARD,       //�I��؂̑��݃`�F�b�N
    SEQ_START_MSG_NOLOCK, //���łɑI��؂������āA�o�g���{�b�N�X�����b�N����Ă��Ȃ�
    SEQ_START_MSG_LOCK,   //���łɑI��؂������āA�o�g���{�b�N�X�����b�N����Ă���

    SEQ_START_MSG_RECVCARD, //�I��؎󂯎��
    SEQ_START_RECVCARD,
    SEQ_WAIT_RECVCARD,
    SEQ_WAIT_RECV_DEBUG,

    SEQ_START_CANCEL,   //�L�����Z��
    SEQ_WAIT_CANCEL,    //�L�����Z��
    SEQ_START_MSG_CANCEL,
    SEQ_START_LIST_CANCEL,
    SEQ_WAIT_LIST_CANCEL,

    SEQ_CHECK_RECVCARD, //�󂯎�����I��؂��`�F�b�N

    SEQ_START_MSG_SAVE, //�I��؂������������̂ŃT�C���A�b�v���ăZ�[�u
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,

    SEQ_START_MSG_DIRTY_CRC,    //�s���ȃ��M�����[�V�����������Ƃ��� CRC�s��
    SEQ_START_MSG_DIRTY_VER,    //�s���ȃ��M�����[�V�����������Ƃ��� VERSION�s��
    SEQ_START_MSG_RETIRE,   //���^�C�A���������󂯎���Ă���,
    SEQ_START_MSG_FINISH,   //�I�����������󂯎���Ă���,

    SEQ_SUCCESS_END,      //���������̂Ŏ���
    SEQ_DIRTY_END,        //���s�����̂Ŗ߂�

    SEQ_WAIT_MSG,       //���b�Z�[�W�҂�
  };

  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHECK_CARD:       //�I��؂̑��݃`�F�b�N
    {
      SAVE_CONTROL_WORK   *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
      BATTLE_BOX_SAVE     *p_btlbox_sv  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv_ctrl );
      switch( Regulation_GetCardParam( p_wk->p_regulation, REGULATION_CARD_STATUS) )
      { 
      case DREAM_WORLD_MATCHUP_NONE:    // �o�^���Ă��Ȃ����
      case DREAM_WORLD_MATCHUP_END:	    // ���I��
      case DREAM_WORLD_MATCHUP_RETIRE:  // �r���ŉ���
        if( Regulation_GetCardParam( p_wk->p_regulation, REGULATION_CARD_CUPNO) == 0 )
        { 
          //���Ȃ��̂Ŏ�M�p��
          *p_seq  = SEQ_START_MSG_RECVCARD;
        }
        else
        { 
          //������̂ŁA���b�N�ɑ����̃��b�Z�[�W���o���Ă����M��
          if( BATTLE_BOX_SAVE_GetLockType( p_btlbox_sv, BATTLE_BOX_LOCK_BIT_LIVE ) )
          { 
            *p_seq  = SEQ_START_MSG_LOCK;
          }
          else
          { 
            *p_seq  = SEQ_START_MSG_NOLOCK;
          }
        }
        break;

      case DREAM_WORLD_MATCHUP_SIGNUP:	  // web�o�^�����BDS->SAKE�̐ڑ��͂܂�
        //���łɎ�M���Ă��邽�ߓo�^������
        WBM_SEQ_SetNext( p_seqwk, SEQFUNC_Register );
        break;

      case DREAM_WORLD_MATCHUP_ENTRY:	  // �Q����
        //�Q�����Ă��邽�ߑ��C����
        WBM_SEQ_SetNext( p_seqwk, SEQFUNC_StartCup );
        break;
      }
    }
    break;
  case SEQ_START_MSG_NOLOCK: //���łɑI��؂������āA�o�g���{�b�N�X�����b�N����Ă��Ȃ�
    UTIL_TEXT_Print( p_wk, LIVE_STR_01 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_MSG_RECVCARD );
    break;
  case SEQ_START_MSG_LOCK:   //���łɑI��؂������āA�o�g���{�b�N�X�����b�N����Ă���
    UTIL_TEXT_Print( p_wk, LIVE_STR_02 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_MSG_RECVCARD );
    break;

  case SEQ_START_MSG_RECVCARD: //�I��؎󂯎��
    UTIL_TEXT_Print( p_wk, LIVE_STR_03 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_RECVCARD );
    break;
  case SEQ_START_RECVCARD:
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, TRUE );
    LIVEBATTLEMATCH_IRC_StartRecvRegulation( p_wk->p_irc, &p_wk->regulation_temp );
    *p_seq  = SEQ_WAIT_RECVCARD;
    break;
  case SEQ_WAIT_RECVCARD:
    //��M��
    if( LIVEBATTLEMATCH_IRC_WaitRecvRegulation( p_wk->p_irc ) )
    { 
      //@todo ����R�[�h���Ⴄ�ꍇ�͂܂����炢�ɍs��

      //Regulation_PrintDebug( &p_wk->regulation_temp );
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      *p_seq  = SEQ_CHECK_RECVCARD;
    }

    //�G���[����
    if( LIVEBATTLEMATCH_IRC_ERROR_REPAIR_DISCONNECT == LIVEBATTLEMATCH_IRC_CheckErrorRepairType( p_wk->p_irc ) )
    { 
      *p_seq  = SEQ_START_MSG_RECVCARD;
      break;
    }

    //B�L�����Z��
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
    { 
      *p_seq  = SEQ_START_CANCEL;
    }

#ifdef DEBUG_REGULATION_RECV_A_PASS
    //A�Ői��
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    { 
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      LIVEBATTLEMATCH_IRC_StartCancelRecvRegulation( p_wk->p_irc );
      *p_seq  = SEQ_WAIT_RECV_DEBUG;
    }
#endif
    break;

  case SEQ_WAIT_RECV_DEBUG:
    if( LIVEBATTLEMATCH_IRC_WaitCancelRecvRegulation( p_wk->p_irc ) )
    { 
      Regulation_SetDebugData( &p_wk->regulation_temp );
      *p_seq  = SEQ_CHECK_RECVCARD;
    }
    break;

  case SEQ_START_CANCEL:   //�L�����Z��
    LIVEBATTLEMATCH_IRC_StartCancelRecvRegulation( p_wk->p_irc );
    *p_seq  = SEQ_WAIT_CANCEL;
    break;
  case SEQ_WAIT_CANCEL:    //�L�����Z��
    if( LIVEBATTLEMATCH_IRC_WaitCancelRecvRegulation( p_wk->p_irc ) )
    { 
      *p_seq  = SEQ_START_MSG_CANCEL;
    }
    break;
  case SEQ_START_MSG_CANCEL:
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
    UTIL_TEXT_Print( p_wk, LIVE_STR_17 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_LIST_CANCEL );
    break;
  case SEQ_START_LIST_CANCEL:
    UTIL_LIST_Create( p_wk, LVM_MENU_TYPE_YESNO ); 
    *p_seq  = SEQ_WAIT_LIST_CANCEL;
    break;
  case SEQ_WAIT_LIST_CANCEL:
    { 
      const u32 select  = UTIL_LIST_Main( p_wk ); 
      if( select != WBM_LIST_SELECT_NULL )
      { 
        UTIL_LIST_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_DIRTY_END;
          break;

        case 1: //������
          *p_seq  = SEQ_START_MSG_RECVCARD;
          break;
        }
      }
    }
    break;

  case SEQ_CHECK_RECVCARD: //�󂯎�����I��؂��`�F�b�N
#ifdef DEBUG_REGULATIONCRC_PASS
    if(1)
#else
    //CRC�͐�������
    if( Regulation_CheckCrc( &p_wk->regulation_temp ) )
#endif
    { 
      //VERSION�͐�������
      if( Regulation_GetCardParam( &p_wk->regulation_temp, REGULATION_CARD_ROMVER) & (1<<GET_VERSION()) )
      { 
        //���łɎ󂯂Ƃ��Ă��邩
        if( Regulation_GetCardParam( p_wk->p_regulation, REGULATION_CARD_CUPNO) ==
            Regulation_GetCardParam( &p_wk->regulation_temp, REGULATION_CARD_CUPNO) )
        { 
#ifdef DEBUG_REGULATION_RECVCHECK_PASS
          { 
            *p_seq  = SEQ_START_MSG_SAVE;
            break;
          }
#endif 
          //�����Ȃ̂œ������ɓo�^���悤�Ƃ���
          switch( Regulation_GetCardParam( p_wk->p_regulation, REGULATION_CARD_STATUS) )
          {   
          case DREAM_WORLD_MATCHUP_RETIRE:  // �r���ŉ���
            *p_seq  = SEQ_START_MSG_RETIRE;
            break;

          case DREAM_WORLD_MATCHUP_END:	    // ���I��
            *p_seq  = SEQ_START_MSG_FINISH;
            break;

          default:
            //��{�����ɂ͂��Ȃ����O�̂���
            //���NO�͓��������Q�����Ȃ̂Ŏ��ւ���
            *p_seq  = SEQ_START_MSG_SAVE;
            break;
          }
        }
        else
        { 
          //���NO���Ⴄ�̂ŁA���v
          *p_seq  = SEQ_START_MSG_SAVE;
        }
      }
      else
      { 
        //VERSION���Ԉ���Ă���
        OS_TPrintf( "�o�[�W�������Ⴄ��\n" );
        *p_seq  = SEQ_START_MSG_DIRTY_VER;
      }
    }
    else
    { 
      //CRC���Ԉ���Ă���
      OS_TPrintf( "CRC���Ⴄ��\n" );
      *p_seq  = SEQ_START_MSG_DIRTY_CRC;
    }
    break;

  case SEQ_START_MSG_SAVE: //�I��؂������������̂ŃT�C���A�b�v���ăZ�[�u
    UTIL_TEXT_Print( p_wk, LIVE_STR_09 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    break;
  case SEQ_START_SAVE: //�I��؂������������̂ŃT�C���A�b�v���ăZ�[�u
    { 
      SAVE_CONTROL_WORK   *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
      REGULATION_SAVEDATA *p_reg_sv   = SaveData_GetRegulationSaveData( p_sv_ctrl );
      
      //�J�Ï�Ԃ��T�C���A�b�v�ɂ��Ă���Z�[�u
      Regulation_SetCardParam( &p_wk->regulation_temp, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_SIGNUP );
      //�V�K���Ȃ̂ŃX�R�A���N���A����
      LIVEMATCH_Init( p_wk->p_livematch );

      OS_TPrintf( "�Z�b�g�O\n" );
      OS_TPrintf( "�e���|����\n" );
      Regulation_PrintDebug( &p_wk->regulation_temp );
      OS_TPrintf( "�Z�[�u\n" );
      Regulation_PrintDebug( p_wk->p_regulation );
      RegulationSaveData_SetRegulation(p_reg_sv, REGULATION_CARD_TYPE_LIVE, &p_wk->regulation_temp);
      OS_TPrintf( "�Z�b�g��\n" );
      OS_TPrintf( "�e���|����\n" );
      Regulation_PrintDebug( &p_wk->regulation_temp );
      OS_TPrintf( "�Z�[�u\n" );
      Regulation_PrintDebug( p_wk->p_regulation );
    }

    GAMEDATA_SaveAsyncStart(p_wk->param.p_gamedata);
    *p_seq  = SEQ_WAIT_SAVE;
    break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT result;
      result = GAMEDATA_SaveAsyncMain(p_wk->param.p_gamedata);
      switch(result){
      case SAVE_RESULT_CONTINUE:
      case SAVE_RESULT_LAST:
        break;
      case SAVE_RESULT_OK:
      case SAVE_RESULT_NG:
        *p_seq  = SEQ_SUCCESS_END;
        break;
      }
    } 
    break;

  case SEQ_START_MSG_DIRTY_CRC:    //�s���ȃ��M�����[�V�����������Ƃ���
    UTIL_TEXT_Print( p_wk, LIVE_STR_22_1 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    break;

  case SEQ_START_MSG_DIRTY_VER:    //�s���ȃ��M�����[�V�����������Ƃ���
    UTIL_TEXT_Print( p_wk, LIVE_STR_22_2 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    break;

  case SEQ_START_MSG_RETIRE:   //���^�C�A���������󂯎���Ă���:
    UTIL_TEXT_Print( p_wk, LIVE_STR_04 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    break;
  case SEQ_START_MSG_FINISH:   //�I�����������󂯎���Ă���:
    UTIL_TEXT_Print( p_wk, LIVE_STR_05 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    break;

  case SEQ_SUCCESS_END:      //���������̂Ŏ���

    OS_TPrintf( "�Z�[�u��\n" );
    Regulation_PrintDebug( p_wk->p_regulation );
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_Register );
    break;
  case SEQ_DIRTY_END:        //���s�����̂Ŗ߂�
    UTIL_FLOW_End( p_wk, LIVEBATTLEMATCH_FLOW_RET_LIVEMENU );
    break;

  case SEQ_WAIT_MSG:       //���b�Z�[�W�҂�
    if( UTIL_TEXT_IsEnd( p_wk ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	���o�^�V�[�P���X
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Register( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_DRAW_CARD,  //�I��؂ƃo�g���{�b�N�X�\��
    SEQ_WAIT_DRAW_CARD,
    SEQ_START_MSG_REG,     //�o�g���{�b�N�X�̃|�P������o�^���܂�
    SEQ_START_LIST_REG_YESNO,
    SEQ_WAIT_LIST_REG_YESNO,

    SEQ_START_MSG_CANCEL,     //�Q���|�P�����̓o�^����߂܂���
    SEQ_START_LIST_CANCEL_YESNO,
    SEQ_WAIT_LIST_CANCEL_YESNO,

    SEQ_MOVE_BTLBOX,      //�o�g���{�b�N�X���E
    SEQ_CHECK_REG,        //�o�g���{�b�N�X�̃|�P���������M�����[�V�����ƈ�v���邩�`�F�b�N
    SEQ_START_MSG_DIRTY_REG,  //���M�����[�V�����ƈ�v���Ȃ�����
    SEQ_LOCK_BTLBOX,      //�o�g���{�b�N�X�����b�N

    SEQ_START_MSG_SAVE,     //�o�^�����Z�[�u
    SEQ_START_SAVE,        
    SEQ_WAIT_SAVE,

    SEQ_START_MSG_COMPLATE, //�������b�Z�[�W

    SEQ_SUCCESS_END,      //���������̂Ŏ���
    SEQ_DIRTY_MOVE_BTLBOX,//���s�����Ƃ��̃o�g���{�b�N�X���E
    SEQ_DIRTY_END,        //���s�����̂Ŗ߂�

    SEQ_WAIT_MSG,       //���b�Z�[�W�҂� 
  };

  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_CARD:  //�I��؂ƃo�g���{�b�N�X�\��
    UTIL_PLAYERINFO_Create( p_wk );
    UTIL_PLAYERINFO_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNREGISTER );
    UTIL_BTLBOX_Create( p_wk );
    *p_seq  = SEQ_WAIT_DRAW_CARD;
    break;
  case SEQ_WAIT_DRAW_CARD:
    {
      BOOL ret = TRUE;
      ret &= UTIL_PLAYERINFO_Move( p_wk );
      ret &= UTIL_BTLBOX_MoveIn( p_wk );
      if( ret )
      { 
        *p_seq  = SEQ_START_MSG_REG;
      }
    }
    break;
  case SEQ_START_MSG_REG:     //�o�g���{�b�N�X�̃|�P������o�^���܂�
    UTIL_TEXT_Print( p_wk, LIVE_STR_06 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_LIST_REG_YESNO );
    break;
  case SEQ_START_LIST_REG_YESNO:
    UTIL_LIST_Create( p_wk, LVM_MENU_TYPE_YESNO ); 
    *p_seq       = SEQ_WAIT_LIST_REG_YESNO;
    break;
  case SEQ_WAIT_LIST_REG_YESNO:
    { 
      const u32 select  = UTIL_LIST_Main( p_wk ); 
      if( select != WBM_LIST_SELECT_NULL )
      { 
        UTIL_LIST_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_MOVE_BTLBOX;
          break;

        case 1: //������
          *p_seq  = SEQ_START_MSG_CANCEL;
          break;
        }
      }
    }
    break;

  case SEQ_START_MSG_CANCEL:     //�Q���|�P�����̓o�^����߂܂���
    UTIL_TEXT_Print( p_wk, LIVE_STR_07 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_LIST_CANCEL_YESNO );
    break;
  case SEQ_START_LIST_CANCEL_YESNO:
    UTIL_LIST_Create( p_wk, LVM_MENU_TYPE_YESNO ); 
    *p_seq       = SEQ_WAIT_LIST_CANCEL_YESNO;
    break;
  case SEQ_WAIT_LIST_CANCEL_YESNO:
    { 
      const u32 select  = UTIL_LIST_Main( p_wk ); 
      if( select != WBM_LIST_SELECT_NULL )
      { 
        UTIL_LIST_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_DIRTY_MOVE_BTLBOX;
          break;

        case 1: //������
          *p_seq  = SEQ_START_MSG_REG;
          break;
        }
      }
    }
    break;

  case SEQ_MOVE_BTLBOX:      //�o�g���{�b�N�X���E
    if( UTIL_BTLBOX_MoveOut( p_wk ) )
    { 
      UTIL_BTLBOX_Delete( p_wk );
      *p_seq  = SEQ_CHECK_REG;
    }
    break;
  case SEQ_CHECK_REG:        //�o�g���{�b�N�X�̃|�P���������M�����[�V�����ƈ�v���邩�`�F�b�N
    { 
      u32 failed_bit  = 0;
      REGULATION      *p_reg  = RegulationData_GetRegulation( p_wk->p_regulation );
      UTIL_DATA_GetBtlBoxParty( p_wk, p_wk->p_party );
      if( POKE_REG_OK == PokeRegulationMatchLookAtPokeParty(p_reg, p_wk->p_party, &failed_bit) )
      {
        OS_TPrintf( "�o�g���{�b�N�X�̃|�P�������M�����[�V����OK\n" );
        *p_seq  = SEQ_LOCK_BTLBOX;
      }
      else
      { 
        OS_TPrintf( "�o�g���{�b�N�X�̃|�P�������M�����[�V����NG!! 0x%x \n", failed_bit );
        *p_seq  = SEQ_START_MSG_DIRTY_REG;
      }
    }
    break;
  case SEQ_START_MSG_DIRTY_REG:  //���M�����[�V�����ƈ�v���Ȃ�����
    UTIL_TEXT_Print( p_wk, LIVE_STR_08 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    break;

  case SEQ_LOCK_BTLBOX:      //�o�g���{�b�N�X�����b�N
    { 
      //�o�g���{�b�N�X�̃��b�N�ƁA���b�N���̃|�P���������Z�[�u����
      SAVE_CONTROL_WORK   *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
      { 
        BATTLE_BOX_SAVE     *p_btlbox_sv  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv_ctrl ); 
        REGULATION_SAVEDATA *p_save = SaveData_GetRegulationSaveData( p_sv_ctrl );
        REGULATION_VIEWDATA *p_view_sv =  RegulationSaveData_GetRegulationView( p_save, BATTLE_BOX_LOCK_BIT_LIVE );
        POKEPARTY *p_party  = BATTLE_BOX_SAVE_MakePokeParty( p_btlbox_sv, GFL_HEAP_LOWID(HEAPID_WIFIBATTLEMATCH_SYS) );

        BATTLE_BOX_SAVE_OnLockFlg( p_btlbox_sv, BATTLE_BOX_LOCK_BIT_LIVE );


        RegulationView_SetEazy( p_view_sv, p_party );

        GFL_HEAP_FreeMemory( p_party );
        
      }
    }
    *p_seq  = SEQ_START_MSG_SAVE;
    break;

  case SEQ_START_MSG_SAVE:     //�o�^�����Z�[�u
    UTIL_PLAYERINFO_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_LOCK );
    UTIL_TEXT_Print( p_wk, LIVE_STR_09 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE ); 
    break;
  case SEQ_START_SAVE:
    //�J�Ï�Ԃ��Q�����ɂ��Ă���Z�[�u
    Regulation_SetCardParam( p_wk->p_regulation, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_ENTRY );
    GAMEDATA_SaveAsyncStart(p_wk->param.p_gamedata);
    *p_seq  = SEQ_WAIT_SAVE;
    break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT result;
      result = GAMEDATA_SaveAsyncMain(p_wk->param.p_gamedata);
      switch(result){
      case SAVE_RESULT_CONTINUE:
      case SAVE_RESULT_LAST:
        break;
      case SAVE_RESULT_OK:
      case SAVE_RESULT_NG:
        *p_seq  = SEQ_START_MSG_COMPLATE;
        break;
      }
    } 
    break;

  case SEQ_START_MSG_COMPLATE: //�������b�Z�[�W
    UTIL_TEXT_Print( p_wk, LIVE_STR_10 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_SUCCESS_END ); 
    break;

  case SEQ_SUCCESS_END:      //���������̂Ŏ���
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_StartCup );
    break;
    
  case SEQ_DIRTY_MOVE_BTLBOX: //���s�����Ƃ��̃o�g���{�b�N�X���E
    if( UTIL_BTLBOX_MoveOut( p_wk ) )
    { 
      UTIL_BTLBOX_Delete( p_wk );
      *p_seq  = SEQ_DIRTY_END;
    }
    break;

  case SEQ_DIRTY_END:        //���s�����̂Ŗ߂�
    UTIL_FLOW_End( p_wk, LIVEBATTLEMATCH_FLOW_RET_LIVEMENU );
    break;

  case SEQ_WAIT_MSG:       //���b�Z�[�W�҂� 
    if( UTIL_TEXT_IsEnd( p_wk ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief	���J�n�V�[�P���X
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_StartCup( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_DRAW_CARD,  //�I��ؕ\��
    SEQ_WAIT_DRAW_CARD,

    SEQ_CHECK_ERR,      //�G���[�����Ė߂��Ă������H
    SEQ_START_MSG_ERR,
    
    SEQ_START_MSG_MENU,   //���C�����j���[
    SEQ_START_LIST_MENU,
    SEQ_WAIT_LIST_MENU,

    SEQ_START_MSG_UNREG,  //�Q������������
    SEQ_START_LIST_UNREG,
    SEQ_WAIT_LIST_UNREG,
    SEQ_START_MSG_DECIDE, //�{���Ɂ`
    SEQ_START_LIST_DECIDE,
    SEQ_WAIT_LIST_DECIDE,
    SEQ_START_MSG_SAVE,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_START_MSG_UNLOCK,
  
    SEQ_START_MSG_LOOKBACK, //�U��Ԃ�

    SEQ_LOOKBACK_END,     //�U��Ԃ邽�߂ɏI��
    SEQ_MATCHING_END,     //�}�b�`���O��
    SEQ_UNREGISTER_END,   //�������ďI������

    SEQ_WAIT_MSG,         //���b�Z�[�W�҂� 
  };

  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_CARD:  //�I��ؕ\��
    UTIL_DATA_SetupMyData( p_wk->param.p_player_data, p_wk );

    if( p_wk->p_playerinfo )
    { 
      *p_seq  = SEQ_START_MSG_MENU;
    }
    else
    { 
      UTIL_PLAYERINFO_Create( p_wk );
      UTIL_PLAYERINFO_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_LOCK );
      *p_seq  = SEQ_WAIT_DRAW_CARD;
    }
		break;
  case SEQ_WAIT_DRAW_CARD:
    if( UTIL_PLAYERINFO_Move( p_wk ) )
    { 
      *p_seq  = SEQ_CHECK_ERR;
    }
		break;

  case SEQ_CHECK_ERR:      //�G���[�����Ė߂��Ă������H
    if( p_wk->param.p_btl_score->is_error )
    { 
      *p_seq  = SEQ_START_MSG_ERR;
    }
    else
    { 
      *p_seq  = SEQ_START_MSG_MENU;
    }
    break;

  case SEQ_START_MSG_ERR:
    UTIL_TEXT_Print( p_wk, LIVE_ERR_01 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_MSG_MENU );
    break;
    
  case SEQ_START_MSG_MENU:   //���C�����j���[
    UTIL_TEXT_Print( p_wk, LIVE_STR_11 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_LIST_MENU );
		break;
  case SEQ_START_LIST_MENU:
    UTIL_LIST_Create( p_wk, LVM_MENU_TYPE_MENU ); 
    *p_seq       = SEQ_WAIT_LIST_MENU;
		break;
  case SEQ_WAIT_LIST_MENU:
    { 
      const u32 select  = UTIL_LIST_Main( p_wk ); 
      if( select != WBM_LIST_SELECT_NULL )
      { 
        UTIL_LIST_Delete( p_wk );
        if( p_wk->is_rec  && !p_wk->param.p_btl_score->is_error )
        { 
          switch( select )
          { 
          case 0: //�������񂷂�
            *p_seq  = SEQ_MATCHING_END;
            break;

          case 1: //�ӂ肩����
            *p_seq  = SEQ_START_MSG_LOOKBACK;
            break;

          case 2: //�Q����������
            *p_seq  = SEQ_START_MSG_UNREG;
            break;
          }
        }
        else
        { 
          switch( select )
          { 
          case 0: //�������񂷂�
            *p_seq  = SEQ_MATCHING_END;
            break;

          case 1: //�Q����������
            *p_seq  = SEQ_START_MSG_UNREG;
            break;
          }
        }
      }
    }
		break;

  case SEQ_START_MSG_UNREG:  //�Q������������
    UTIL_TEXT_Print( p_wk, LIVE_STR_13 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_LIST_UNREG );
		break;
  case SEQ_START_LIST_UNREG:
    UTIL_LIST_Create( p_wk, LVM_MENU_TYPE_YESNO ); 
    *p_seq  = SEQ_WAIT_LIST_UNREG;
		break;
  case SEQ_WAIT_LIST_UNREG:
    { 
      const u32 select  = UTIL_LIST_Main( p_wk ); 
      if( select != WBM_LIST_SELECT_NULL )
      { 
        UTIL_LIST_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_START_MSG_DECIDE;
          break;

        case 1: //������
          *p_seq  = SEQ_START_MSG_MENU;
          break;
        }
      }
    }
		break;
  case SEQ_START_MSG_DECIDE: //�{���Ɂ`
    UTIL_TEXT_Print( p_wk, LIVE_STR_14 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_LIST_DECIDE );
		break;
  case SEQ_START_LIST_DECIDE:
    UTIL_LIST_Create( p_wk, LVM_MENU_TYPE_YESNO ); 
    *p_seq  = SEQ_WAIT_LIST_DECIDE;
		break;
  case SEQ_WAIT_LIST_DECIDE:
    { 
      const u32 select  = UTIL_LIST_Main( p_wk ); 
      if( select != WBM_LIST_SELECT_NULL )
      { 
        UTIL_LIST_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_START_MSG_SAVE;
          break;

        case 1: //������
          *p_seq  = SEQ_START_MSG_MENU;
          break;
        }
      }
    }
		break;
  case SEQ_START_MSG_SAVE:
    UTIL_TEXT_Print( p_wk, LIVE_STR_09 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE ); 
		break;
  case SEQ_START_SAVE:
    //�������Ă���Z�[�u
    Regulation_SetCardParam( p_wk->p_regulation, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_RETIRE );
    { 
      SAVE_CONTROL_WORK   *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
      BATTLE_BOX_SAVE     *p_btlbox_sv  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv_ctrl ); 
      BATTLE_BOX_SAVE_OffLockFlg( p_btlbox_sv, BATTLE_BOX_LOCK_BIT_LIVE );
    }

    GAMEDATA_SaveAsyncStart(p_wk->param.p_gamedata);
    *p_seq  = SEQ_WAIT_SAVE;
		break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT result;
      result = GAMEDATA_SaveAsyncMain(p_wk->param.p_gamedata);
      switch(result){
      case SAVE_RESULT_CONTINUE:
      case SAVE_RESULT_LAST:
        break;
      case SAVE_RESULT_OK:
      case SAVE_RESULT_NG:
        *p_seq  = SEQ_START_MSG_UNLOCK;
        break;
      }
    } 
		break;
  case SEQ_START_MSG_UNLOCK:
    UTIL_PLAYERINFO_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK );
    UTIL_TEXT_Print( p_wk, LIVE_STR_15 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_UNREGISTER_END );
		break;
  
  case SEQ_START_MSG_LOOKBACK: //�U��Ԃ�
    UTIL_TEXT_Print( p_wk, LIVE_STR_12 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_LOOKBACK_END );
		break;

  case SEQ_LOOKBACK_END:     //�U��Ԃ邽�߂ɏI��
    UTIL_FLOW_End( p_wk, LIVEBATTLEMATCH_FLOW_RET_BTLREC );
		break;
  case SEQ_MATCHING_END:     //�}�b�`���O��
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_Matching );
		break;
  case SEQ_UNREGISTER_END:   //�������ďI������
    UTIL_FLOW_End( p_wk, LIVEBATTLEMATCH_FLOW_RET_LIVEMENU );
		break;

  case SEQ_WAIT_MSG:         //���b�Z�[�W�҂� 
    if( UTIL_TEXT_IsEnd( p_wk ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    } 
		break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�`���O�����V�[�P���X
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG_MATCHING, //�}�b�`���O�J�n
    SEQ_START_CONNECT,      //�ڑ��J�n
    SEQ_WAIT_CONNECT,       

    SEQ_START_CANCEL_MATCHING,
    SEQ_WAIT_CANCEL_MATCHING,
    SEQ_START_MSG_CANCEL_MATCHING, //�}�b�`���O��߂�
    SEQ_START_LIST_CANCEL_MATCHING,
    SEQ_START_LSIT_CANCEL_MATCHING,

    SEQ_SEND_ENEMYDATA,      //������
    SEQ_RECV_ENEMYDATA,      //������
    SEQ_CHECK_MATCH,      //�}�b�`���O����
    SEQ_START_DISCONNECT_NOCUP,  //���Ⴄ���ߐؒf
    SEQ_WAIT_DISCONNECT_NOCUP,
    SEQ_START_MSG_NOCUP,  //���Ⴄ���b�Z�[�W
    SEQ_START_MSG_OK,     //�}�b�`���O���肪���������I
    SEQ_START_MSG_CNT,
    SEQ_START_DRAW_OTHERCARD,
    SEQ_WAIT_DRAW_OTHERCARD,
    SEQ_WAIT_SYNC,
    SEQ_START_MSG_WAIT,
    SEQ_START_TIMING,
    SEQ_WAIT_TIMING,

    SEQ_CHECK_SHOW,       //�݂������͂��邩
    SEQ_LIST_END,         //���X�g��

    SEQ_WAIT_MSG,         //���b�Z�[�W�҂� 
  };

  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;


  //�G���[����
  if( LIVEBATTLEMATCH_IRC_ERROR_REPAIR_DISCONNECT == LIVEBATTLEMATCH_IRC_CheckErrorRepairType( p_wk->p_irc ) )
  { 
    p_wk->param.p_btl_score->is_error = TRUE;
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_StartCup );
  }

  //�V�[�P���X
  switch( *p_seq )
  { 
  case SEQ_START_MSG_MATCHING: //�}�b�`���O�J�n
    UTIL_TEXT_Print( p_wk, LIVE_STR_16 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_CONNECT ); 
		break;
  case SEQ_START_CONNECT:
    if( LIVEBATTLEMATCH_IRC_StartConnect( p_wk->p_irc ) )
    { 
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, TRUE );

      *p_seq  = SEQ_WAIT_CONNECT;
    }
    break;
  case SEQ_WAIT_CONNECT:
    if( LIVEBATTLEMATCH_IRC_WaitConnect( p_wk->p_irc ) )
    { 
      *p_seq  = SEQ_SEND_ENEMYDATA;
    }

    //�L�����Z������
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
    { 
      *p_seq  = SEQ_START_CANCEL_MATCHING;
    }
    break;

  case SEQ_START_CANCEL_MATCHING:
    LIVEBATTLEMATCH_IRC_StartCancelConnect( p_wk->p_irc );
    *p_seq  = SEQ_WAIT_CANCEL_MATCHING;
		break;
  case SEQ_WAIT_CANCEL_MATCHING:
    if( LIVEBATTLEMATCH_IRC_WaitCancelConnect( p_wk->p_irc ) )
    { 
      *p_seq  = SEQ_START_MSG_CANCEL_MATCHING;
    }
		break;
  case SEQ_START_MSG_CANCEL_MATCHING: //�}�b�`���O��߂�
    UTIL_TEXT_Print( p_wk, LIVE_STR_17 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_LIST_CANCEL_MATCHING );
		break;
  case SEQ_START_LIST_CANCEL_MATCHING:
    UTIL_LIST_Create( p_wk, LVM_MENU_TYPE_YESNO ); 
    *p_seq       = SEQ_START_LSIT_CANCEL_MATCHING;
    break;
  case SEQ_START_LSIT_CANCEL_MATCHING:
    { 
      const u32 select  = UTIL_LIST_Main( p_wk ); 
      if( select != WBM_LIST_SELECT_NULL )
      { 
        UTIL_LIST_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, SEQFUNC_StartCup );
          break;

        case 1: //������
          *p_seq  = SEQ_START_CONNECT;
          break;
        }
      }
    }
    break;

  case SEQ_SEND_ENEMYDATA:      //������
    if( LIVEBATTLEMATCH_IRC_StartEnemyData( p_wk->p_irc, p_wk->param.p_player_data ) )
    { 
      *p_seq  = SEQ_RECV_ENEMYDATA;
    }
    break;
  case SEQ_RECV_ENEMYDATA:      //������
    { 
      WIFIBATTLEMATCH_ENEMYDATA *p_buff_adrs; 
      if( LIVEBATTLEMATCH_IRC_WaitEnemyData( p_wk->p_irc, &p_buff_adrs ) )
      { 
        GFL_STD_MemCopy( p_buff_adrs, p_wk->param.p_enemy_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
        *p_seq  = SEQ_CHECK_MATCH;
      }
    }
    break;
  case SEQ_CHECK_MATCH:      //�}�b�`���O����
    if( p_wk->param.p_player_data->wificup_no == p_wk->param.p_enemy_data->wificup_no )
    { 
      OS_TPrintf( "�����̑�� %d ����̑�� %d\n", p_wk->param.p_player_data->wificup_no, p_wk->param.p_enemy_data->wificup_no );
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      *p_seq  = SEQ_START_MSG_OK;
    }
    else
    { 
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      *p_seq  = SEQ_START_DISCONNECT_NOCUP;
    }
		break;

  case SEQ_START_DISCONNECT_NOCUP:  //���Ⴄ���ߐؒf
    if( LIVEBATTLEMATCH_IRC_StartDisConnect(p_wk->p_irc))
    { 
      *p_seq  = SEQ_WAIT_DISCONNECT_NOCUP;
    }
    break;
  case SEQ_WAIT_DISCONNECT_NOCUP:
    if( LIVEBATTLEMATCH_IRC_WaitDisConnect(p_wk->p_irc))
    { 
      *p_seq  = SEQ_START_MSG_NOCUP;
    }
    break;
  case SEQ_START_MSG_NOCUP:  //���Ⴄ���b�Z�[�W
    UTIL_TEXT_Print( p_wk, LIVE_STR_18 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_MSG_MATCHING );
		break;
  case SEQ_START_MSG_OK:     //�}�b�`���O���肪���������I
    UTIL_TEXT_Print( p_wk, LIVE_STR_19 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_MSG_CNT );
		break;
  case SEQ_START_MSG_CNT:
    if( p_wk->cnt++ > MATCHING_MSG_WAIT_SYNC )
    { 
      p_wk->cnt      = 0;
      GFL_BG_SetVisible( BG_FRAME_M_TEXT, FALSE );
      *p_seq      = SEQ_START_DRAW_OTHERCARD;
    }
    break;
  case SEQ_START_DRAW_OTHERCARD:
    UTIL_MATCHINFO_Create( p_wk, p_wk->param.p_enemy_data );
    *p_seq  = SEQ_WAIT_DRAW_OTHERCARD;
		break;
  case SEQ_WAIT_DRAW_OTHERCARD:
    if( UTIL_MATCHINFO_Main( p_wk ) )
    { 
      *p_seq  = SEQ_WAIT_SYNC;
    }
		break;
  case SEQ_WAIT_SYNC:
    if( p_wk->cnt++ > ENEMYDATA_WAIT_SYNC )
    {  
      *p_seq  = SEQ_START_MSG_WAIT;
    }
		break;

  case SEQ_START_MSG_WAIT:
    GFL_BG_SetVisible( BG_FRAME_M_TEXT, TRUE );
    UTIL_TEXT_Print( p_wk, LIVE_STR_20 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_TIMING );
		break;
    
  case SEQ_START_TIMING:
    GFL_NET_HANDLE_TimeSyncStart( GFL_NET_HANDLE_GetCurrentHandle(),71, WB_NET_IRC_BATTLE );
    *p_seq  = SEQ_WAIT_TIMING;
    break;

  case SEQ_WAIT_TIMING:
    if( GFL_NET_HANDLE_IsTimeSync( GFL_NET_HANDLE_GetCurrentHandle(),71, WB_NET_IRC_BATTLE) )
    { 
      *p_seq  = SEQ_CHECK_SHOW;
    }
    break;

  case SEQ_CHECK_SHOW:        //���������͂��邩
    { 
      REGULATION      *p_reg  = RegulationData_GetRegulation( p_wk->p_regulation );
      if( Regulation_GetParam( p_reg, REGULATION_SHOW_POKE) )
      { 
        //����
        *p_seq  = SEQ_LIST_END;
      }
      else
      { 
        //�Ȃ�
        *p_seq  = SEQ_LIST_END;
      }
    }
    break;

  case SEQ_LIST_END:         //���X�g��
    UTIL_FLOW_End( p_wk, LIVEBATTLEMATCH_FLOW_RET_BATTLE );
		break;

  case SEQ_WAIT_MSG:         //���b�Z�[�W�҂� 
    if( UTIL_TEXT_IsEnd( p_wk ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
		break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�퓬��V�[�P���X
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_BtlAfter( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_DISCONNECT,
    SEQ_WAIT_DISCONNECT,
    SEQ_START_MSG_SAVE,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_REC_END,  //�^���

    SEQ_WAIT_MSG,         //���b�Z�[�W�҂� 
  };

  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;

  //�G���[����
  if( LIVEBATTLEMATCH_IRC_ERROR_REPAIR_DISCONNECT == LIVEBATTLEMATCH_IRC_CheckErrorRepairType( p_wk->p_irc ) )
  { 
    p_wk->param.p_btl_score->is_error = TRUE;
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_StartCup );
  }

  switch( *p_seq )
  {
  case SEQ_START_DISCONNECT:
    if( LIVEBATTLEMATCH_IRC_StartDisConnect(p_wk->p_irc))
    { 
      *p_seq  = SEQ_WAIT_DISCONNECT;
    }
    break;
  case SEQ_WAIT_DISCONNECT:
    if( LIVEBATTLEMATCH_IRC_WaitDisConnect(p_wk->p_irc))
    { 
      *p_seq  = SEQ_START_MSG_SAVE;
    }
    break;
  case SEQ_START_MSG_SAVE:
    UTIL_TEXT_Print( p_wk, LIVE_STR_09 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
		break;
  case SEQ_START_SAVE:

    //�ΐ푊��̏��Z�[�u
    { 
      u8 idx  = LIVEMATCH_DATA_GetMyParam( p_wk->p_livematch, LIVEMATCH_MYDATA_PARAM_BTLCNT );
      LIVEMATCH_DATA_SetFoeMacAddr( p_wk->p_livematch, idx, p_wk->param.p_enemy_data->mac_address );
      LIVEMATCH_DATA_SetFoeParam( p_wk->p_livematch, idx, LIVEMATCH_FOEDATA_PARAM_REST_POKE, p_wk->param.p_btl_score->enemy_rest_poke );
      LIVEMATCH_DATA_SetFoeParam( p_wk->p_livematch, idx, LIVEMATCH_FOEDATA_PARAM_REST_HP, p_wk->param.p_btl_score->enemy_rest_hp );

      OS_TPrintf( "���ΐ푊��̃f�[�^\n" );
      OS_TPrintf( "mac_addr [%s]\n", p_wk->param.p_enemy_data->mac_address );
      OS_TPrintf( "rest_poke[%d]\n", p_wk->param.p_btl_score->enemy_rest_poke );
      OS_TPrintf( "rest_hp  [%d]\n", p_wk->param.p_btl_score->enemy_rest_hp );
    }

    //�����̃X�R�A�Z�[�u
    { 
      u8  win  = 0;
      u8  lose  = 0;
      switch( p_wk->param.p_btl_score->result )
      { 
      case BTL_RESULT_RUN_ENEMY:
        /* fallthrough */
      case BTL_RESULT_WIN:
        win = 1;
        break;
      case BTL_RESULT_RUN:
        /* fallthrough */
      case BTL_RESULT_LOSE:
        lose  = 1;
        break;
      case BTL_RESULT_DRAW:
        break;
      default:
        GF_ASSERT_MSG(0, "�o�g�����ʕs���l %d\n", p_wk->param.p_btl_score->result );
      }

      LIVEMATCH_DATA_AddMyParam( p_wk->p_livematch, LIVEMATCH_MYDATA_PARAM_WIN,    win );
      LIVEMATCH_DATA_AddMyParam( p_wk->p_livematch, LIVEMATCH_MYDATA_PARAM_LOSE,   lose );
      LIVEMATCH_DATA_AddMyParam( p_wk->p_livematch, LIVEMATCH_MYDATA_PARAM_BTLCNT, 1 );
    }

    GAMEDATA_SaveAsyncStart(p_wk->param.p_gamedata);
    *p_seq  = SEQ_WAIT_SAVE;
		break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT result;
      result = GAMEDATA_SaveAsyncMain(p_wk->param.p_gamedata);
      switch(result){
      case SAVE_RESULT_CONTINUE:
      case SAVE_RESULT_LAST:
        break;
      case SAVE_RESULT_OK:
      case SAVE_RESULT_NG:
        *p_seq  = SEQ_REC_END;
        break;
      }
    } 
		break;
  case SEQ_REC_END:  //�^���
    UTIL_FLOW_End( p_wk, LIVEBATTLEMATCH_FLOW_RET_REC );
		break;

  case SEQ_WAIT_MSG:         //���b�Z�[�W�҂� 
    if( UTIL_TEXT_IsEnd( p_wk ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
		break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�^���V�[�P���X
 *
 *	@param	WBM_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_RecAfter( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK_LAST,   //�Ō�̂ӂ肩���肩���`�F�b�N
    SEQ_CHECK_BTLCNT, //�K��񐔐����
    SEQ_START_MSG_SAVE,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_START_MSG_UNLOCK,
    SEQ_START_MSG_LOOKBACK,
    SEQ_START_LIST_LOOKBACK,
    SEQ_WAIT_LIST_LOOKBACK,
    SEQ_START_MSG_END,

    SEQ_LOOKBACK_END, //�U��Ԃ�
    SEQ_END,      //�ΐ�I��
    SEQ_RETURN,   //�܂��킦��

    SEQ_WAIT_MSG,         //���b�Z�[�W�҂� 
  };
  LIVEBATTLEMATCH_FLOW_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHECK_LAST:  //�Ō�̐U��Ԃ肩���`�F�b�N
    if( Regulation_GetCardParam( p_wk->p_regulation, REGULATION_CARD_STATUS) == DREAM_WORLD_MATCHUP_END )
    { 
      *p_seq  = SEQ_START_MSG_LOOKBACK;
    }
    else
    { 
      *p_seq  = SEQ_CHECK_BTLCNT;
    }
    break;

  case SEQ_CHECK_BTLCNT: //�K��񐔐����
    {
      REGULATION  *p_reg  = RegulationData_GetRegulation( p_wk->p_regulation );
      u32 now = LIVEMATCH_DATA_GetMyParam( p_wk->p_livematch, LIVEMATCH_MYDATA_PARAM_BTLCNT );
      u32 max = Regulation_GetParam( p_reg, REGULATION_BTLCOUNT);
      if( now >= max )
      { 
        *p_seq  = SEQ_START_MSG_SAVE;
      }
      else
      { 
        *p_seq  = SEQ_RETURN;
      }
    }
		break;
  case SEQ_START_MSG_SAVE:
    UTIL_TEXT_Print( p_wk, LIVE_STR_09 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    break;
  case SEQ_START_SAVE:
    //�J�Ï�Ԃ��I���ɂ��Ă���Z�[�u
    Regulation_SetCardParam( p_wk->p_regulation, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_END );

    //���b�N�������Ă���Z�[�u
    { 
      SAVE_CONTROL_WORK   *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
      BATTLE_BOX_SAVE     *p_btlbox_sv  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv_ctrl ); 
      BATTLE_BOX_SAVE_OffLockFlg( p_btlbox_sv, BATTLE_BOX_LOCK_BIT_LIVE );
    }
    GAMEDATA_SaveAsyncStart(p_wk->param.p_gamedata);
    *p_seq  = SEQ_WAIT_SAVE;
		break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT result;
      result = GAMEDATA_SaveAsyncMain(p_wk->param.p_gamedata);
      switch(result){
      case SAVE_RESULT_CONTINUE:
      case SAVE_RESULT_LAST:
        break;
      case SAVE_RESULT_OK:
      case SAVE_RESULT_NG:
        *p_seq  = SEQ_START_MSG_UNLOCK;
        break;
      }
    } 
		break;

  case SEQ_START_MSG_UNLOCK:
    UTIL_PLAYERINFO_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK );
    UTIL_TEXT_Print( p_wk, LIVE_STR_21 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_MSG_LOOKBACK );
    break;
    
  case SEQ_START_MSG_LOOKBACK:
    UTIL_TEXT_Print( p_wk, LIVE_STR_23 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_LIST_LOOKBACK );
    break;
  case SEQ_START_LIST_LOOKBACK:
    UTIL_LIST_Create( p_wk, LVM_MENU_TYPE_END );
    *p_seq  = SEQ_WAIT_LIST_LOOKBACK;
    break;

  case SEQ_WAIT_LIST_LOOKBACK:
    { 
      const u32 select  = UTIL_LIST_Main( p_wk ); 
      if( select != WBM_LIST_SELECT_NULL )
      { 
        UTIL_LIST_Delete( p_wk );
        if( p_wk->is_rec  && !p_wk->param.p_btl_score->is_error )
        { 
          switch( select )
          { 
          case 0: //�ӂ肩����
            *p_seq  = SEQ_LOOKBACK_END;
            break;

          case 1: //�����
            *p_seq  = SEQ_START_MSG_END;
            break;
          }
        }
        else
        { 
          switch( select )
          { 
          case 0: //�����
            *p_seq  = SEQ_START_MSG_END;
            break;
          }
        }
      }
    }
    break;

  case SEQ_START_MSG_END:
    UTIL_TEXT_Print( p_wk, LIVE_STR_24 );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_END );
    break;
  case SEQ_LOOKBACK_END:      //�U��Ԃ�
    UTIL_FLOW_End( p_wk, LIVEBATTLEMATCH_FLOW_RET_BTLREC );
		break;
  case SEQ_END:      //�ΐ�I��
    UTIL_FLOW_End( p_wk, LIVEBATTLEMATCH_FLOW_RET_LIVEMENU );
		break;
  case SEQ_RETURN:   //�܂��킦��
    WBM_SEQ_SetNext( p_seqwk, SEQFUNC_StartCup );
		break;

  case SEQ_WAIT_MSG:         //���b�Z�[�W�҂� 
    if( UTIL_TEXT_IsEnd( p_wk ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
		break;
  }
}


//=============================================================================
/**
 *  �֗��֐�
 */
//=============================================================================
//-------------------------------------
///	���X�g
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g��\��
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *	@param	type                                �\�����X�g�^�C�v
 */
//-----------------------------------------------------------------------------
static void UTIL_LIST_Create( LIVEBATTLEMATCH_FLOW_WORK *p_wk, LVM_MENU_TYPE type )
{ 
  enum
  { 
    POS_CENTER,
    POS_RIGHT_DOWN,
  } pos;

  u8 x,y,w,h;

  WBM_LIST_SETUP  setup;
  GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
  setup.p_msg   = p_wk->p_msg;
  setup.p_font  = p_wk->param.p_font;
  setup.p_que   = p_wk->param.p_que;


  switch( type )
  { 
  case LVM_MENU_TYPE_YESNO:
    setup.strID[0]= LIVE_SELECT_00;
    setup.strID[1]= LIVE_SELECT_01;
    setup.list_max= 2;
    pos = POS_RIGHT_DOWN;
    break;

  case LVM_MENU_TYPE_MENU:
    if( p_wk->is_rec  && !p_wk->param.p_btl_score->is_error )
    { 
      setup.strID[0]= LIVE_SELECT_02;
      setup.strID[1]= LIVE_SELECT_03;
      setup.strID[2]= LIVE_SELECT_04;
      setup.list_max= 3;
    }
    else
    { 
      setup.strID[0]= LIVE_SELECT_02;
      setup.strID[1]= LIVE_SELECT_04;
      setup.list_max= 2;
    }
    pos = POS_CENTER;
    break;
  case LVM_MENU_TYPE_END:
    if( p_wk->is_rec  && !p_wk->param.p_btl_score->is_error )
    { 
      setup.strID[0]= LIVE_SELECT_03;
      setup.strID[1]= LIVE_SELECT_05;
      setup.list_max= 2;
    }
    else
    { 
      setup.strID[0]= LIVE_SELECT_05;
      setup.list_max= 1;
    }
    pos = POS_CENTER;
    break;
  }
  setup.frm     = BG_FRAME_M_TEXT;
  setup.font_plt= PLT_FONT_M;
  setup.frm_plt = PLT_LIST_M;
  setup.frm_chr = CGR_OFS_M_LIST;


  switch( pos )
  { 
  case POS_CENTER:
    w  = 28;
    h  = setup.list_max * 2;
    x  = 32 / 2  - w /2; 
    y  = (24-6) /2  - h /2;
    break;
  case POS_RIGHT_DOWN:
    w  = 12;
    h  = setup.list_max * 2;
    x  = 32 - w - 1; //1�̓t���[����
    y  = 24 - h - 1 - 6; //�P�͎����̃t���[������6�̓e�L�X�g��
    break;
  }

  p_wk->p_list  = WBM_LIST_InitEx( &setup, x, y, w, h, p_wk->heapID );
  p_wk->list_type = type;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g���폜
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UTIL_LIST_Delete( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{ 
  WBM_LIST_Exit( p_wk->p_list );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g����
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *
 *	@retval �I�����C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static u32 UTIL_LIST_Main( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{ 
  u32 ret = WBM_LIST_Main( p_wk->p_list );

  if( ret == WBM_LIST_SELECT_CALNCEL )
  { 
    if( p_wk->list_type == LVM_MENU_TYPE_YESNO )
    { 
      ret = 1;  //NO�ɂ���
    }
    else
    { 
      //�L�����Z�������Ȃ�
      ret = WBM_LIST_SELECT_NULL;
    }
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�����\��
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *	@param	strID                               ����
 */
//-----------------------------------------------------------------------------
static void UTIL_TEXT_Print( LIVEBATTLEMATCH_FLOW_WORK *p_wk, u32 strID )
{ 
  WBM_TEXT_Print( p_wk->param.p_text, p_wk->p_msg, strID, WBM_TEXT_TYPE_STREAM );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�\���҂�
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *
 *	@return TRUE�����`�抮��  FALSE�����`�撆
 */
//-----------------------------------------------------------------------------
static BOOL UTIL_TEXT_IsEnd( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{ 
  return WBM_TEXT_IsEnd( p_wk->param.p_text );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t���[�I��
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *	@param	ret                                 �߂�l
 */
//-----------------------------------------------------------------------------
static void UTIL_FLOW_End( LIVEBATTLEMATCH_FLOW_WORK *p_wk, LIVEBATTLEMATCH_FLOW_RET ret )
{ 
  p_wk->retcode = ret;
  WBM_SEQ_SetNext( p_wk->p_seq, SEQFUNC_End );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h�쐬
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk   ���[�N
 */
//-----------------------------------------------------------------------------
static void UTIL_PLAYERINFO_Create( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{ 
  if( p_wk->p_playerinfo == NULL )
  {
    MYSTATUS    *p_my;
    GFL_CLUNIT	*p_unit;

    //�����̃f�[�^��\��
    PLAYERINFO_LIVECUP_DATA info_setup;
    SAVE_CONTROL_WORK   *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
    BATTLE_BOX_SAVE     *p_btlbox_sv  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv_ctrl );
    REGULATION  *p_reg  = RegulationData_GetRegulation(p_wk->p_regulation);
    REGULATION_SAVEDATA *p_reg_sv   = SaveData_GetRegulationSaveData( p_sv_ctrl );
    REGULATION_VIEWDATA *p_reg_view = RegulationSaveData_GetRegulationView( p_reg_sv, REGULATION_CARD_TYPE_LIVE ); 

    p_my  = GAMEDATA_GetMyStatus( p_wk->param.p_gamedata); 
    p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->param.p_graphic );

    GFL_STD_MemClear( &info_setup, sizeof(PLAYERINFO_LIVECUP_DATA) );

    //Regulation_PrintDebug( p_wk->p_regulation );

    info_setup.win_cnt  = LIVEMATCH_DATA_GetMyParam( p_wk->p_livematch, LIVEMATCH_MYDATA_PARAM_WIN );
    info_setup.lose_cnt = LIVEMATCH_DATA_GetMyParam( p_wk->p_livematch, LIVEMATCH_MYDATA_PARAM_LOSE );
    info_setup.btl_cnt  = LIVEMATCH_DATA_GetMyParam( p_wk->p_livematch, LIVEMATCH_MYDATA_PARAM_BTLCNT );
    info_setup.btl_max  = Regulation_GetParam( p_reg, REGULATION_BTLCOUNT );
    info_setup.trainerID  = MyStatus_GetTrainerView( p_my );

    GFL_STD_MemCopy( Regulation_GetCardCupNamePointer( p_wk->p_regulation ),
        info_setup.cup_name, 2*(WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE) );

    info_setup.start_date = GFDATE_Set( 
        Regulation_GetCardParam( p_wk->p_regulation, REGULATION_CARD_START_YEAR ),
        Regulation_GetCardParam( p_wk->p_regulation, REGULATION_CARD_START_MONTH ), 
        Regulation_GetCardParam( p_wk->p_regulation, REGULATION_CARD_START_DAY ),
          0);

    info_setup.end_date = GFDATE_Set( 
        Regulation_GetCardParam( p_wk->p_regulation, REGULATION_CARD_END_YEAR ),
        Regulation_GetCardParam( p_wk->p_regulation, REGULATION_CARD_END_MONTH ), 
        Regulation_GetCardParam( p_wk->p_regulation, REGULATION_CARD_END_DAY ),
          0);

    p_wk->p_playerinfo	= PLAYERINFO_LIVE_Init( &info_setup, p_my, p_unit, p_wk->param.p_view, p_wk->param.p_font, p_wk->param.p_que, p_wk->p_wifi_msg, p_wk->p_word, p_reg_view, p_wk->heapID );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h�j��
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UTIL_PLAYERINFO_Delete( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{ 
  if( p_wk->p_playerinfo )
  { 
    PLAYERINFO_RND_Exit( p_wk->p_playerinfo );
    p_wk->p_playerinfo  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h���X���C�h�C��
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL UTIL_PLAYERINFO_Move( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{
  return PLAYERINFO_MoveMain( p_wk->p_playerinfo );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h�̃f�[�^�X�V
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *	@param	type                            �X�V�^�C�v
 */
//-----------------------------------------------------------------------------
static void UTIL_PLAYERINFO_RenewalData( LIVEBATTLEMATCH_FLOW_WORK *p_wk, PLAYERINFO_WIFI_UPDATE_TYPE type )
{ 
  PLAYERINFO_LIVE_RenewalData( p_wk->p_playerinfo, type, p_wk->p_wifi_msg, p_wk->param.p_que, p_wk->param.p_font, p_wk->heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�҂̃J�[�h���쐬
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk     ���[�N
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *cp_enemy_data  �ΐ�ҏ��
 */
//-----------------------------------------------------------------------------
static void UTIL_MATCHINFO_Create( LIVEBATTLEMATCH_FLOW_WORK *p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_enemy_data )
{
  if( p_wk->p_matchinfo == NULL )
  { 
    GFL_CLUNIT  *p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->param.p_graphic );
    p_wk->p_matchinfo		= MATCHINFO_Init( cp_enemy_data, p_unit, p_wk->param.p_view, p_wk->param.p_font, p_wk->param.p_que, p_wk->p_wifi_msg, p_wk->p_word, WIFIBATTLEMATCH_TYPE_LIVECUP, FALSE, p_wk->heapID );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�҂̃J�[�h��j��
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UTIL_MATCHINFO_Delete( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{
  if( p_wk->p_matchinfo )
  { 
    MATCHINFO_Exit( p_wk->p_matchinfo );
    p_wk->p_matchinfo = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�҂̃J�[�h���X���C�h�C��
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL UTIL_MATCHINFO_Main( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{
  return MATCHINFO_MoveMain( p_wk->p_matchinfo );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X  �쐬
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UTIL_BTLBOX_Create( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{
  if( p_wk->p_btlbox == NULL )
  { 
    GFL_CLUNIT  *p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->param.p_graphic );
    SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
    BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );

#ifdef PM_DEBUG
    { 
      int i;
      POKEPARTY *p_party  = BATTLE_BOX_SAVE_MakePokeParty( p_bbox_save, p_wk->heapID );
      for( i = 0; i < PokeParty_GetPokeCount(p_party); i++ )
      { 
        POKEMON_PARAM *p_pp = PokeParty_GetMemberPointer( p_party, i );
        if( PP_Get( p_pp, ID_PARA_poke_exist, NULL ) )
        { 
          NAGI_Printf( "monsno[%d]\n",  PP_Get( p_pp, ID_PARA_monsno, NULL ) );
          NAGI_Printf( "formno[%d]\n",  PP_Get( p_pp, ID_PARA_form_no, NULL ) );
          NAGI_Printf( "item[%d]\n",  PP_Get( p_pp, ID_PARA_item, NULL ) );
        }
      }
      GFL_HEAP_FreeMemory( p_party );
    }
#endif //PM_DEBUG

    p_wk->p_btlbox  = WBM_BTLBOX_Init( p_bbox_save, p_unit, p_wk->param.p_view, p_wk->p_small_font, p_wk->param.p_que, p_wk->p_wifi_msg, p_wk->heapID );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X  �j��
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UTIL_BTLBOX_Delete( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{
  if( p_wk->p_btlbox )
  { 
    WBM_BTLBOX_Exit( p_wk->p_btlbox );
    p_wk->p_btlbox  = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X  �X���C�h�C��
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL UTIL_BTLBOX_MoveIn( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{
  return WBM_BTLBOX_MoveInMain( p_wk->p_btlbox );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X  �X���C�h�A�E�g
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL UTIL_BTLBOX_MoveOut( LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{
  return WBM_BTLBOX_MoveOutMain( p_wk->p_btlbox );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�p�[�e�B�e���|�����փo�g���{�b�N�X�̃p�[�e�B���Z�b�g
 *
 *	@param	LIVEBATTLEMATCH_FLOW_WORK *p_wk ���[�N
 *	@param  POKEPARTY                       �󂯎��
 */
//-----------------------------------------------------------------------------
static void UTIL_DATA_GetBtlBoxParty( LIVEBATTLEMATCH_FLOW_WORK *p_wk, POKEPARTY *p_dst )
{ 
  SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->param.p_gamedata );
  BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
  POKEPARTY         *p_party  =  BATTLE_BOX_SAVE_MakePokeParty( p_bbox_save, p_wk->heapID );
  GFL_STD_MemCopy( p_party, p_dst, PokeParty_GetWorkSize() );
  GFL_HEAP_FreeMemory( p_party );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃f�[�^���i�[
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data  �i�[�ꏊ
 *	@param	*p_wk                                 ���[�N
 */
//-----------------------------------------------------------------------------
static void UTIL_DATA_SetupMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, LIVEBATTLEMATCH_FLOW_WORK *p_wk )
{ 
  GAMEDATA  *p_gamedata  = p_wk->param.p_gamedata;
  //SERVER
  p_my_data->btl_server_version  = BTL_NET_SERVER_VERSION;
  //MYSTATUS
  {
    MYSTATUS  *p_my;
    p_my  = GAMEDATA_GetMyStatus(p_gamedata);
    GFL_STD_MemCopy( p_my, p_my_data->mystatus, MyStatus_GetWorkSize() );
  }
  //MAC�A�h���X
  { 
    OS_GetMacAddress( p_my_data->mac_address );
  }
  //PMS
  { 
    const MYPMS_DATA *cp_mypms;
    SAVE_CONTROL_WORK *p_sv;
    p_sv            = GAMEDATA_GetSaveControlWork(p_gamedata);
    cp_mypms        = SaveData_GetMyPmsDataConst( p_sv );
    MYPMS_GetPms( cp_mypms, MYPMS_PMS_TYPE_INTRODUCTION, &p_my_data->pms );
  }
  //�p�[�e�B
  {
    SAVE_CONTROL_WORK*	p_sv	= GAMEDATA_GetSaveControlWork(p_gamedata);
    BATTLE_BOX_SAVE *p_btlbox_sv = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
    POKEPARTY *p_temoti  = BATTLE_BOX_SAVE_MakePokeParty( p_btlbox_sv, GFL_HEAP_LOWID(HEAPID_WIFIBATTLEMATCH_SYS) );
    GFL_STD_MemCopy( p_temoti, p_my_data->pokeparty, PokeParty_GetWorkSize() );
    GFL_HEAP_FreeMemory( p_temoti );
  }
  //�X�R�A
  { 
    p_my_data->win_cnt  = LIVEMATCH_DATA_GetMyParam( p_wk->p_livematch, LIVEMATCH_MYDATA_PARAM_WIN );
    p_my_data->lose_cnt = LIVEMATCH_DATA_GetMyParam( p_wk->p_livematch, LIVEMATCH_MYDATA_PARAM_LOSE );
    p_my_data->btl_cnt  = LIVEMATCH_DATA_GetMyParam( p_wk->p_livematch, LIVEMATCH_MYDATA_PARAM_BTLCNT );
  }
  //���ԍ�
  { 
    p_my_data->wificup_no = Regulation_GetCardParam( p_wk->p_regulation, REGULATION_CARD_STATUS);
  }
}
