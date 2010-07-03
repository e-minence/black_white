//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_wifi.c
 *	@brief	WIFI�̃o�g���}�b�`�R�A���  WIFI�V�[�P���X
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
#include "system/bmp_winframe.h"
#include "pokeicon/pokeicon.h"
#include "gamesystem/game_data.h"
#include "system/net_err.h"
#include "poke_tool/poke_regulation.h"
#include "sound/pm_sndsys.h"
#include "net/dwc_rapcommon.h"
#include "net/dwc_tool.h"
#include "battle/btl_net.h" //BTL_NET_SERVER_VERSION

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
#include "savedata/battle_box_save.h"
#include "savedata/my_pms_data.h"
#include "savedata/etc_save.h"

//WIFI�o�g���}�b�`�̃��W���[��
#include "wifibattlematch_graphic.h"
#include "wifibattlematch_view.h"
#include "wifibattlematch_net.h"
#include "wifibattlematch_util.h"
#include "wifibattlematch_snd.h"

//�f�o�b�O
#include "wifibattlematch_debug.h"

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


#if defined(DEBUG_ONLY_FOR_toru_nagihashi)||defined(DEBUG_ONLY_FOR_shimoyamada)
#define GPF_FLAG_ON             //GPF�t���O������ON�ɂ���
#endif

//#define MYPOKE_SELFCHECK        //�����̃|�P�����𑗂����Ƃ��A�T�P�ƃ`�F�b�N���������ؖ�������
//#define DEBUG_REGULATION_DATA   //���M�����[�V�����f�[�^���쐬����
//#define REGULATION_CHECK_ON     //�p�[�e�B�̃��M�����[�V�����`�F�b�N������ON�ɂ���

#define DEBUG_REG_CRC_CHECK_OFF

#define DEBUGWIN_USE
#endif //PM_DEBUG



#define DEBUG_WIFICUP_Printf(...)  OS_TFPrintf( 2, __VA_ARGS__ );

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�T�u�V�[�P���X�̖߂�l
//=====================================
typedef enum
{
  //�o�^���������̖߂�l
  WBM_WIFI_SUBSEQ_UNREGISTER_RET_TRUE  = 0,//�o�^��������
  WBM_WIFI_SUBSEQ_UNREGISTER_RET_FALSE,   //�������Ȃ�����
  WBM_WIFI_SUBSEQ_UNREGISTER_RET_NONE,   //�o�^����Ă��Ȃ�
  WBM_WIFI_SUBSEQ_UNREGISTER_RET_SERVER,   //�T�[�o�[��Ԃ�����

  //�����ԃ`�F�b�N�̖߂�l
  WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMESAFE  = 0,//���ԓ�
  WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEOVER,     //���Ԍ�
  WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEBEFORE,   //���ԑO
  WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER,   //�T�[�o�[��Ԃ�����

  //�|�P�����s���`�F�b�N�̖߂�l
  WBM_WIFI_SUBSEQ_EVILCHECK_RET_SUCCESS  = 0,  //����
  WBM_WIFI_SUBSEQ_EVILCHECK_RET_DARTY,         //�P�̂ł��s���|�P����������
  WBM_WIFI_SUBSEQ_EVILCHECK_RET_NET_ERR,       //�l�b�g�G���[


  WBM_WIFI_SUBSEQ_RET_NONE  = 0xFFFF,       //�Ȃ�
} WBM_WIFI_SUBSEQ_RET;

//-------------------------------------
///	�V���N
//=====================================
#define ENEMYDATA_WAIT_SYNC (0)
#define MATCHING_MSG_WAIT_SYNC (120)
#define SELECTPOKE_MSG_WAIT_SYNC (60)

#define MATCHING_TIMEOUT_SYNC   (60*20)

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
  // ------ �ȉ����W���[�� ------
	//�O���t�B�b�N�ݒ�
	WIFIBATTLEMATCH_GRAPHIC_WORK	*p_graphic;

  //���\�[�X�ǂݍ���
  WIFIBATTLEMATCH_VIEW_RESOURCE *p_res;

	//���ʂŎg���t�H���g
	GFL_FONT			            *p_font;

  //�|�P�����̃��x���E���ʃ}�[�N�\���݂̂Ŏg�p���Ă���t�H���g
	GFL_FONT			            *p_small_font;

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

  //�o�g���{�b�N�X
  WBM_BTLBOX_WORK           *p_btlbox;

  //�ҋ@�A�C�R��
  WBM_WAITICON_WORK         *p_wait;

  //�e�L�X�g��
  WBM_TEXT_WORK             *p_text;

  //���X�g
  WBM_LIST_WORK             *p_list;

  //�l�b�g
  WIFIBATTLEMATCH_NET_WORK  *p_net;

  //���C���V�[�P���X
  WBM_SEQ_WORK              *p_seq;

  //�T�u�V�[�P���X
  WBM_SEQ_WORK              *p_subseq;
  WBM_WIFI_SUBSEQ_RET       subseq_ret;


  // ------ �ȉ��f�[�^ ------
  //�s���`�F�b�N�̂Ƃ��Ɏg�p����o�b�t�@�i����ȊO�͎Q�Ƃ��Ȃ��j
  WIFIBATTLEMATCH_NET_EVILCHECK_DATA  evilecheck_data;

  //�J�E���^
  u32 cnt;

  //�s���J�E���^
  u32 other_dirty_cnt;
  u32 my_dirty_cnt;

  //�L�����Z���V�[�P���X
  u32 cancel_seq;
  u32 match_timeout;

  BOOL is_send_report;

  //�T�[�o�[���痎�Ƃ��Ă����}�b�`���O����̓o�^�|�P����
  POKEPARTY *p_other_party;

  //����
  WIFIBATTLEMATCH_CORE_PARAM  *p_param;

  REGULATION_CARDDATA         *p_reg;
  REGULATION_CARDDATA         recv_card;

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
//static void WbmWifiSeq_Join( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );  
////�ȑO�̓��j���[�͂��̃\�[�X���ł��Ă��܂������A
//battle_championship�̒��ł����Ȃ����ƂɂȂ�܂����O�̂��ߏ����Ă͂��܂���WbmWifiSeq_Join�͂Ȃ����Ă��܂���B
static void WbmWifiSeq_RecvDigCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_CheckDigCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Register( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_EndRec( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_CupContinue( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_DisConnextSendTime( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_DisConnextEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_DisConnextCheckDate( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Err_ReturnLogin( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	�T�u�V�[�P���X�֐��i�V�[�P���X��œ������́j
//=====================================
static void WbmWifiSubSeq_CheckDate( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSubSeq_UnRegister( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSubSeq_EvilCheck( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	���W���[�����g���₷���܂Ƃ߂�����
//=====================================
//�v���C���[���
static void Util_PlayerInfo_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static void Util_PlayerInfo_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static BOOL Util_PlayerInfo_MoveIn( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static BOOL Util_PlayerInfo_MoveOut( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static void Util_PlayerInfo_CreateStay( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static void Util_PlayerInfo_RenewalData( WIFIBATTLEMATCH_WIFI_WORK *p_wk, PLAYERINFO_WIFI_UPDATE_TYPE type );
//�ΐ푊����
static void Util_MatchInfo_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_enemy_data );
static void Util_MatchInfo_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static BOOL Util_MatchInfo_Main( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static void Util_Matchinfo_Clear( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
//�o�g���{�b�N�X
static void Util_BtlBox_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static void Util_BtlBox_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static BOOL Util_BtlBox_MoveIn( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static BOOL Util_BtlBox_MoveOut( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
//�I����
typedef enum
{ 
  UTIL_LIST_TYPE_YESNO,
  UTIL_LIST_TYPE_JOIN,
  UTIL_LIST_TYPE_DECIDE,
  UTIL_LIST_TYPE_CUPMENU,
  UTIL_LIST_TYPE_CUPMENU_CONT,
  UTIL_LIST_TYPE_YESNO_DEFAULT_NO,
}UTIL_LIST_TYPE;
static void Util_List_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk, UTIL_LIST_TYPE type );
static void Util_List_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static u32 Util_List_Main( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
//�T�u�V�[�P���X
static void Util_SubSeq_Start( WIFIBATTLEMATCH_WIFI_WORK *p_wk, WBM_SEQ_FUNCTION seq_function );
static WBM_WIFI_SUBSEQ_RET Util_SubSeq_Main( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
//�����̃f�[�^�쐬
static void Util_InitMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static void Util_SetMyDataInfo( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, const WIFIBATTLEMATCH_WIFI_WORK *cp_wk );
static void Util_SetMyDataSign( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, const WIFIBATTLEMATCH_NET_EVILCHECK_DATA  *cp_evilecheck_data );
//�|�P�����ؖ�
static BOOL Util_VerifyPokeData( WIFIBATTLEMATCH_ENEMYDATA *p_data, POKEPARTY *p_party, HEAPID heapID );
//�|�P�p�[�e�B���r
static BOOL Util_ComarepPokeParty( const POKEPARTY *cp_pokeparty_1, const POKEPARTY *cp_pokeparty_2 );
//�L�����Z���V�[�P���X
typedef enum
{
  UTIL_CANCEL_STATE_NONE, //�������Ă��Ȃ�
  UTIL_CANCEL_STATE_WAIT, //�L�����Z���ҋ@���i�͂��A�������҂����j
  UTIL_CANCEL_STATE_EXIT, //�ҋ@�����𔲂���
  UTIL_CANCEL_STATE_DECIDE, //�L�����Z�����m��
}UTIL_CANCEL_STATE;
static UTIL_CANCEL_STATE Util_Cancel_Seq( WIFIBATTLEMATCH_WIFI_WORK *p_wk, BOOL is_cancel_enable, BOOL can_disconnect );
//�X�R�A�Z�[�u
static BOOL Util_SaveScore( WIFIBATTLEMATCH_WIFI_WORK *p_wk, const WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA *cp_data );

//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef DEBUGWIN_USE
#include "debug/debugwin_sys.h"
#define DEBUGWIN_GROUP_WIFIMATCH (50)

static void DEBUGWIN_Init( WIFIBATTLEMATCH_WIFI_WORK *p_wk, HEAPID heapID );
static void DEBUGWIN_Exit( WIFIBATTLEMATCH_WIFI_WORK *p_wk );

#else

#define DEBUGWIN_Init( ... )  /*  */
#define DEBUGWIN_Exit( ... )  /*  */
#endif //DEBUGWIN_USE

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	WifiBattleMatchWifi_ProcData =
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

  { 
    SAVE_CONTROL_WORK *p_sv       = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
    REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
    p_wk->p_reg    = RegulationSaveData_GetRegulationCard( p_reg_sv, REGULATION_CARD_TYPE_WIFI );
  }

	//�O���t�B�b�N�ݒ�
	p_wk->p_graphic	= WIFIBATTLEMATCH_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_WIFIBATTLEMATCH_CORE );

  //���\�[�X�ǂݍ���
  p_wk->p_res     = WIFIBATTLEMATCH_VIEW_LoadResource( WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic ), WIFIBATTLEMATCH_VIEW_RES_MODE_WIFI, HEAPID_WIFIBATTLEMATCH_CORE );

	//���ʃ��W���[���̍쐬
	p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_small_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_num_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
												NARC_message_wifi_match_dat, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_word	= WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_COUNTRY_BUFLEN, HEAPID_WIFIBATTLEMATCH_CORE );

	//���W���[���̍쐬
  p_wk->p_net   = WIFIBATTLEMATCH_NET_Init( 
      p_wk->p_param->p_net_data,
      p_param->p_param->p_game_data, p_param->p_svl_result, HEAPID_WIFIBATTLEMATCH_CORE );
  p_wk->p_text  = WBM_TEXT_Init( BG_FRAME_M_TEXT, PLT_FONT_M, PLT_TEXT_M, CGR_OFS_M_TEXT, p_wk->p_que, p_wk->p_font, HEAPID_WIFIBATTLEMATCH_CORE );
  p_wk->p_seq   = WBM_SEQ_Init( p_wk, WbmWifiSeq_Init, HEAPID_WIFIBATTLEMATCH_CORE );
  p_wk->p_subseq   = WBM_SEQ_Init( p_wk, NULL, HEAPID_WIFIBATTLEMATCH_CORE );

  { 
    GFL_CLUNIT  *p_unit;
    p_unit  = WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    p_wk->p_wait  = WBM_WAITICON_Init( p_unit, p_wk->p_res, HEAPID_WIFIBATTLEMATCH_CORE );
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
  }

  p_wk->p_other_party     = PokeParty_AllocPartyWork( HEAPID_WIFIBATTLEMATCH_CORE );

  WIFIBATTLEMATCH_NETICON_SetDraw( p_wk->p_net, TRUE );

  //�f�o�b�O�E�B���h�E
  DEBUGWIN_Init( p_wk, HEAPID_WIFIBATTLEMATCH_CORE );

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

	WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	= p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_param_adrs;

  //�f�o�b�O�E�B���h�E
  DEBUGWIN_Exit( p_wk );

  GFL_HEAP_FreeMemory( p_wk->p_other_party );

  WIFIBATTLEMATCH_NETICON_SetDraw( p_wk->p_net, FALSE );

	//���W���[���̔j��
  WBM_WAITICON_Exit( p_wk->p_wait );
  WBM_SEQ_Exit( p_wk->p_subseq );
  WBM_SEQ_Exit( p_wk->p_seq );
  WBM_TEXT_Exit( p_wk->p_text );
  if( p_wk->p_net )
  { 
    WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );
  }
  Util_BtlBox_Delete( p_wk );
  Util_MatchInfo_Delete( p_wk );
  Util_PlayerInfo_Delete( p_wk );
  Util_List_Delete( p_wk );

	//���ʃ��W���[���̔j��
	WORDSET_Delete( p_wk->p_word );
	GFL_MSG_Delete( p_wk->p_msg );
	PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_small_font );
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

  //�ҋ@�A�C�R��
  WBM_WAITICON_Main( p_wk->p_wait );

  //BG
  WIFIBATTLEMATCH_VIEW_Main( p_wk->p_res );

  //NET
  WIFIBATTLEMATCH_NET_Main( p_wk->p_net );

  //�����\��
  if( p_wk->p_playerinfo )
  { 
    PLAYERINFO_PrintMain( p_wk->p_playerinfo, p_wk->p_que );
  }
  if( p_wk->p_matchinfo )
  { 
    MATCHINFO_PrintMain( p_wk->p_matchinfo, p_wk->p_que );
  }
  if( p_wk->p_btlbox )
  { 
    WBM_BTLBOX_PrintMain( p_wk->p_btlbox, p_wk->p_que );
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
 *	@brief  WIFI���  ����������
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Init( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  DEBUG_WIFICUP_Printf( " mode%d ret%d\n", p_param->mode, p_param->retmode );
  switch( p_param->mode )
  { 
  case WIFIBATTLEMATCH_CORE_MODE_START:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_RecvDigCard );
    break;

  case WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR:
    WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
    /* fallthrough */

  case WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE:
    Util_PlayerInfo_CreateStay( p_wk );
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_EndBattle );
    break;

  case WIFIBATTLEMATCH_CORE_MODE_ENDREC:
    Util_PlayerInfo_CreateStay( p_wk );
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_EndRec );
    break;
  }
}
#if 0
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  �Q���I������
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
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
    Util_List_Create( p_wk, UTIL_LIST_TYPE_JOIN );
    *p_seq     = SEQ_WAIT_JOIN_LIST;
    break;
  case SEQ_WAIT_JOIN_LIST:   //�҂�
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
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
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq     = SEQ_WAIT_CANCEL_LIST;
    break;
  case SEQ_WAIT_CANCEL_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
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
#endif
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  �T�[�o��̃f�W�^���I��؃f�[�^���_�E�����[�h����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_RecvDigCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_SEARCH_MSG,

    SEQ_START_DOWNLOAD_GPF_DATA,
    SEQ_WAIT_DOWNLOAD_GPF_DATA,
  
    SEQ_CHECK_SIGNIN,
    SEQ_START_NONE_MSG,
    SEQ_WAIT_NONE_MSG,

    SEQ_INIT_SAKE,
    SEQ_WAIT_SAKE,

    SEQ_END
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_SEARCH_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_04, WBM_TEXT_TYPE_WAIT );
    *p_seq  = SEQ_START_DOWNLOAD_GPF_DATA;
    break;

    //-------------------------------------
    ///	GPF�f�[�^�擾
    //=====================================
  case SEQ_START_DOWNLOAD_GPF_DATA:
    GFL_STD_MemClear( p_wk->p_param->p_gpf_data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA) );
    WIFIBATTLEMATCH_NET_StartRecvGpfData( p_wk->p_net, HEAPID_WIFIBATTLEMATCH_CORE );
    *p_seq = SEQ_WAIT_DOWNLOAD_GPF_DATA;
    break;

  case SEQ_WAIT_DOWNLOAD_GPF_DATA:
    { 
      WIFIBATTLEMATCH_RECV_GPFDATA_RET  ret;

      ret = WIFIBATTLEMATCH_NET_WaitRecvGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_RECV_GPFDATA_RET_SUCCESS )
      { 
        WIFIBATTLEMATCH_NET_GetRecvGpfData( p_wk->p_net, p_wk->p_param->p_gpf_data );
        *p_seq = SEQ_CHECK_SIGNIN;
      }
      else if( ret == WIFIBATTLEMATCH_RECV_GPFDATA_RET_DIRTY )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }
      else if( ret != WIFIBATTLEMATCH_RECV_GPFDATA_RET_UPDATE )
      { 
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��

          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	�T�C���C���`�F�b�N
    //=====================================
  case SEQ_CHECK_SIGNIN:
    if( p_wk->p_param->p_gpf_data->signin )
    { 
      *p_seq = SEQ_INIT_SAKE;
    }
    else
    { 
      *p_seq  = SEQ_START_NONE_MSG;
    }
    break;

  case SEQ_START_NONE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_05, WBM_TEXT_TYPE_STREAM );
    *p_seq  = SEQ_WAIT_NONE_MSG;
    break;

  case SEQ_WAIT_NONE_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    {
      //GPF�ɓo�^���Ă��Ȃ��Ƃ��͓����𑗂�Ȃ����߂��̂܂܏I��
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextEnd );
    }
    break;

    //-------------------------------------
    ///	SAKE���R�[�hID�擾���Ȃ������ꍇ�̃��R�[�h�쐬����
    //=====================================
  case SEQ_INIT_SAKE:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_NET_StartInitialize( p_wk->p_net );
      *p_seq  = SEQ_WAIT_SAKE;
    }
    break;

  case SEQ_WAIT_SAKE:
    {
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_NET_WaitInitialize( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_END;
      }
      
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��

          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CheckDigCard );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  �f�W�^���I��؃f�[�^�`�F�b�N����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_CheckDigCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    //WEB�������݃t���O�`�F�b�N
    SEQ_CHECK_WRITE,
    SEQ_SEND_GPF_STATUS,
    SEQ_WAIT_GPF_STATUS,

    //���`�F�b�N
    SEQ_CHECK_DATA,
    SEQ_CHECK_STATUS,
    SEQ_START_END_MSG,
    SEQ_START_GIVEUP_MSG,
    SEQ_START_WRITE_GPF,
    SEQ_WAIT_WRITE_GPF,
    SEQ_START_WRITE_SAKE_DELETE_POKE,
    SEQ_WAIT_WRITE_SAKE_DELETE_POKE,

    //����
    SEQ_START_MSG_CUPDATA,
    SEQ_START_DOWNLOAD_REG,
    SEQ_WAIT_DOWNLOAD_REG,
    SEQ_WAIT_DOWNLOAD_REG_SUCCESS,
    SEQ_WAIT_DOWNLOAD_REG_FAILURE,
    SEQ_START_SUBSEQ_CUPDATE,
    SEQ_WAIT_SUBSEQ_CUPDATE,
    SEQ_START_NOTCUP_MSG,

    //�o�g���{�b�N�X���b�N�m�F
    SEQ_CHECK_EXITS_REG,

    SEQ_START_MOVEIN_CARD,
    SEQ_WAIT_MOVEIN_CARD,

    SEQ_CHECK_LOCK,

    SEQ_START_BOX_REWRITE_MSG,
    SEQ_START_BOX_REWRITE_YESNO,
    SEQ_SELECT_BOX_REWRITE_YESNO,      // �㏑���m�F
    SEQ_START_BOX_REWRITE_CANCEL_MSG,
    SEQ_START_BOX_REWRITE_CANCEL_YESNO,
    SEQ_SELECT_BOX_REWRITE_CANCEL_YESNO,

    SEQ_START_BOX_LOCK_MSG,           //  �ʏ탍�b�N�m�F
    SEQ_START_BOX_LOCK_YESNO,
    SEQ_SELECT_BOX_LOCK_YESNO,
    SEQ_START_BOX_LOCK_CANCEL_MSG,
    SEQ_START_BOX_LOCK_CANCEL_YESNO,
    SEQ_SELECT_BOX_LOCK_CANCEL_YESNO,

    SEQ_START_MOVEOUT_CARD,
    SEQ_WAIT_MOVEOUT_CARD,

    SEQ_START_UPDATE_MSG,
    SEQ_SEND_GPF_POKEMON,
    SEQ_WAIT_GPF_POKEMON,
    SEQ_SEND_GPF_CUPSTATUS,
    SEQ_WAIT_GPF_CUPSTATUS,
    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE_UPDATE,
    SEQ_WAIT_SAVE_UPDATE,
    SEQ_START_SAVE_NG_MSG,

    //������
    SEQ_START_RECV_SAKE_MSG,
    SEQ_RECV_SAKE_DATA,
    SEQ_WAIT_SAKE_DATA,
    SEQ_CHECK_RATING,
    SEQ_START_RENEWAL_MSG,
    SEQ_START_SAVE_RENEWAL,
    SEQ_WAIT_SAVE_RENEWAL,
    
    SEQ_REGISTER_EXIT,  //�I��؃`�F�b�N���P�ɂ��ďI��
    SEQ_ALREADY_EXIT,   //�I��؃`�F�b�N���Q�ɂ��ďI��
    SEQ_NG_EXIT,        //�I��؃`�F�b�N���O�ɂ��ďI��

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  const REGULATION_CARDDATA *cp_reg_card  = p_wk->p_reg;


  switch( *p_seq )
  { 

    //-------------------------------------
    ///	WEB�������݃t���O�`�F�b�N
    //=====================================
  case SEQ_CHECK_WRITE:
    if( p_wk->p_param->p_gpf_data->GPFEntryFlg == DREAM_WORLD_ENTRYFLAG_GPF_WRITE )
    { 
      //WEB���������񂾂Ƃ������Ƃ́A�K���V�K���Ȃ̂ŁA
      //�������݃t���O�Ə�Ԃ����Z�b�g
      *p_seq  = SEQ_SEND_GPF_STATUS;
    }
    else
    { 
      *p_seq  = SEQ_CHECK_DATA;
    }
    break;

  case SEQ_SEND_GPF_STATUS:
    { 
      DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA data;
      GFL_STD_MemClear( &data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      data.WifiMatchUpState = DREAM_WORLD_MATCHUP_NONE;
      data.GPFEntryFlg      = p_wk->p_param->p_gpf_data->GPFEntryFlg;
      p_wk->p_param->p_gpf_data->WifiMatchUpState = DREAM_WORLD_MATCHUP_NONE;
      WIFIBATTLEMATCH_NET_StartSendGpfData( p_wk->p_net, &data, HEAPID_WIFIBATTLEMATCH_CORE );
    }
    *p_seq  = SEQ_WAIT_GPF_STATUS;
    break;

  case SEQ_WAIT_GPF_STATUS:
    { 
      WIFIBATTLEMATCH_SEND_GPFDATA_RET ret;
      
      ret = WIFIBATTLEMATCH_NET_WaitSendGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS )
      { 
        *p_seq  = SEQ_START_MSG_CUPDATA;
      }
      else if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      if( ret != WIFIBATTLEMATCH_SEND_GPFDATA_RET_UPDATE )
      {
        ///	�I��؃`�F�b�N�ŃG���[���N��������K�����O�C��������Ȃ���
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

    //-------------------------------------
    ///	���Ǝ����̏�Ԃ��`�F�b�N
    //=====================================
  case SEQ_CHECK_DATA:
    //�J�Ã��[�N�`�F�b�N
    if( p_wk->p_param->p_gpf_data->WifiMatchUpState == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS ) )
    { 
      //��v���Ă���
      DEBUG_WIFICUP_Printf( "���[�N��v\n" );
      *p_seq  = SEQ_CHECK_STATUS;
    }
    else
    { 
      //��v���Ă��Ȃ�

      //�V���`���Sor�T�Ȃ�΁A�������݂ɍs��
      //����ȊO�Ȃ�Ώ㏑��
      if( DREAM_WORLD_MATCHUP_RETIRE == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS )
          || DREAM_WORLD_MATCHUP_CHANGE_DS == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS ) )
      { 
        DEBUG_WIFICUP_Printf( "���[�N��v���ĂȂ���߂Ă��� %d\n",  Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS ));
        *p_seq  = SEQ_START_WRITE_GPF;
      }
      else
      { 
        DEBUG_WIFICUP_Printf( "���[�N��v���ĂȂ�\n" );
        *p_seq  = SEQ_CHECK_STATUS;
      }
    }
    break;

  case SEQ_CHECK_STATUS:
    if( p_wk->p_param->p_gpf_data->WifiMatchUpState == DREAM_WORLD_MATCHUP_END )
    { 
      //���I�����Ă���̂�
      //�����̃��b�Z�[�W�������āA������Ȃ������A��
      *p_seq  = SEQ_START_END_MSG;
    }
    else if( p_wk->p_param->p_gpf_data->WifiMatchUpState == DREAM_WORLD_MATCHUP_RETIRE
        || p_wk->p_param->p_gpf_data->WifiMatchUpState == DREAM_WORLD_MATCHUP_CHANGE_DS )
    { 
      //���̑��ɃM�u�A�b�vor�{�̑ւ����Ă���̂ŁA
      //�����̃��b�Z�[�W�������āA������Ȃ������A��
      *p_seq  = SEQ_START_GIVEUP_MSG;
    }
    else
    { 
      //����
      *p_seq  = SEQ_START_MSG_CUPDATA;
    }
    break;

  case SEQ_START_END_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_07, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_RECV_SAKE_MSG );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_GIVEUP_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_07, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_RECV_SAKE_MSG );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_WRITE_GPF:
    { 
      DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA data;
      GFL_STD_MemClear( &data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      data.WifiMatchUpState = Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS );
      p_wk->p_param->p_gpf_data->WifiMatchUpState = Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS );
      WIFIBATTLEMATCH_NET_StartSendGpfData( p_wk->p_net, &data, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_seq  = SEQ_WAIT_WRITE_GPF;
    }
    break;

  case SEQ_WAIT_WRITE_GPF:
    { 
      WIFIBATTLEMATCH_SEND_GPFDATA_RET ret;
      
      ret = WIFIBATTLEMATCH_NET_WaitSendGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS )
      { 
        *p_seq  = SEQ_START_WRITE_SAKE_DELETE_POKE;
      }
      else if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      if( ret != WIFIBATTLEMATCH_SEND_GPFDATA_RET_UPDATE )
      {
        ///	�I��؃`�F�b�N�ŃG���[���N��������K�����O�C��������Ȃ���
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

  case SEQ_START_WRITE_SAKE_DELETE_POKE:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      GFL_STD_MemClear( p_wk->p_param->p_wifi_sake_data->pokeparty, WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE );
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_POKEPARTY, p_wk->p_param->p_wifi_sake_data->pokeparty );
      *p_seq  = SEQ_WAIT_WRITE_SAKE_DELETE_POKE;
    }
    break;

  case SEQ_WAIT_WRITE_SAKE_DELETE_POKE:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_START_GIVEUP_MSG;
      }

      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        ///	�I��؃`�F�b�N�ŃG���[���N��������K�����O�C��������Ȃ���
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

    //-------------------------------------
    ///	�����������̂ŁA�_�E�����[�h�J�n
    //=====================================
  case SEQ_START_MSG_CUPDATA:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_36, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_DOWNLOAD_REG );
    *p_seq  = SEQ_WAIT_MSG;
    break;
  case SEQ_START_DOWNLOAD_REG:
    WIFIBATTLEMATCH_NET_StartDownloadDigCard( p_wk->p_net, p_wk->p_param->p_gpf_data->WifiMatchUpID );
    *p_seq  = SEQ_WAIT_DOWNLOAD_REG;
    break;

  case SEQ_WAIT_DOWNLOAD_REG:
    { 
      WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET  ret;
      ret = WIFIBATTLEMATCH_NET_WaitDownloadDigCard( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_SUCCESS )
      { 
        //�擾����
        WIFIBATTLEMATCH_NET_GetDownloadDigCard( p_wk->p_net, &p_wk->recv_card );

        if( Regulation_CheckCrc(&p_wk->recv_card ) )
        {
          //CRC��v
          *p_seq  =  SEQ_WAIT_DOWNLOAD_REG_SUCCESS;
        }
        else
        {
          //CRC�s��v
          DEBUG_WIFICUP_Printf( "CRC���s��v�������I\n" );
          *p_seq  =  SEQ_WAIT_DOWNLOAD_REG_FAILURE;
        }
      }
      else if( ret == WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_EMPTY )
      { 
        //�擾���s
        DEBUG_WIFICUP_Printf( "���M�����[�V�������Ȃ�������I\n" );
        *p_seq  =  SEQ_WAIT_DOWNLOAD_REG_FAILURE;
      }

      if( ret != WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_DOWNLOADING )
      {
        ///	�I��؃`�F�b�N�ŃG���[���N��������K�����O�C��������Ȃ���
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

  case SEQ_WAIT_DOWNLOAD_REG_FAILURE:
    //�܂��������Ȃ���������ԃ`�F�b�N���s��Ȃ�
    if(0 == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_CUPNO) )
    { 
      *p_seq = SEQ_START_NOTCUP_MSG;
    }
    else
    { 
      *p_seq  = SEQ_START_SUBSEQ_CUPDATE;
    }
    break;

  case SEQ_START_SUBSEQ_CUPDATE:
    {
      ///	�I��؃`�F�b�N�ŃG���[���N��������K�����O�C��������Ȃ���
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        return;

      case WIFIBATTLEMATCH_NET_ERROR_NONE:
        Util_SubSeq_Start( p_wk, WbmWifiSubSeq_CheckDate );
        *p_seq  = SEQ_WAIT_SUBSEQ_CUPDATE;
        break;
      }
    }
    break;

  case SEQ_WAIT_SUBSEQ_CUPDATE:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret != WBM_WIFI_SUBSEQ_RET_NONE )
      { 
        *p_seq  = SEQ_START_RECV_SAKE_MSG;

        {
          ///	�I��؃`�F�b�N�ŃG���[���N��������K�����O�C��������Ȃ���
          switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
          { 
          case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
          case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
            WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
            return;
          }
        }
      }
      if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }
    }
    break;

  case SEQ_START_NOTCUP_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_09, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_NG_EXIT );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_WAIT_DOWNLOAD_REG_SUCCESS:
    if( DREAM_WORLD_MATCHUP_NONE == p_wk->p_param->p_gpf_data->WifiMatchUpState )
    { 
      *p_seq  = SEQ_CHECK_EXITS_REG;
    }
    else if( DREAM_WORLD_MATCHUP_SIGNUP == p_wk->p_param->p_gpf_data->WifiMatchUpState )
    { 
      *p_seq  = SEQ_REGISTER_EXIT;
    }
    else if( DREAM_WORLD_MATCHUP_ENTRY == p_wk->p_param->p_gpf_data->WifiMatchUpState )
    { 
      *p_seq  = SEQ_ALREADY_EXIT;
    }
    else
    { 
      *p_seq  = SEQ_NG_EXIT;
    }
    break;

    //-------------------------------------
    /// �o�g���{�b�N�X���b�N�m�F
    //=====================================
  case SEQ_CHECK_EXITS_REG:
    if( 0 == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_CUPNO ) )
    { 
      *p_seq  = SEQ_START_UPDATE_MSG;
    }
    else
    { 
      //�㏑���m�F�̂��߂ɈȑO�̑I��؂��o��
      *p_seq  = SEQ_START_MOVEIN_CARD;
    }
    break;
    
  case SEQ_START_MOVEIN_CARD:
    Util_PlayerInfo_Create( p_wk );
    {
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      if( BATTLE_BOX_SAVE_GetLockType( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI ) )
      {
        //���b�N����Ă���Ό��\��
        Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_LOCK );
      }
      else
      {
        //���b�N����Ă��Ȃ���Ό���\��
        Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK );
      }
    }
    *p_seq  = SEQ_WAIT_MOVEIN_CARD;
    break;

  case SEQ_WAIT_MOVEIN_CARD:
    if( Util_PlayerInfo_MoveIn( p_wk ) )
    { 
      *p_seq  = SEQ_CHECK_LOCK;
    }
    break;

  case SEQ_CHECK_LOCK:
    {
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      if( BATTLE_BOX_SAVE_GetLockType( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI ) )
      { 
        *p_seq  = SEQ_START_BOX_REWRITE_MSG;
      }
      else
      { 
        *p_seq  = SEQ_START_BOX_LOCK_MSG;
      }
    }
    break;

  //�㏑���m�F
  case SEQ_START_BOX_REWRITE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_11, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_BOX_REWRITE_YESNO );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_BOX_REWRITE_YESNO:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_BOX_REWRITE_YESNO;
    break;

  case SEQ_SELECT_BOX_REWRITE_YESNO:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_START_MOVEOUT_CARD;
          break;
        case 1: //������
          *p_seq  = SEQ_START_BOX_REWRITE_CANCEL_MSG;
          break;
        }
      }
    }
    break;

  case SEQ_START_BOX_REWRITE_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_12, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_BOX_REWRITE_CANCEL_YESNO );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_BOX_REWRITE_CANCEL_YESNO:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_BOX_REWRITE_CANCEL_YESNO;
    break;

  case SEQ_SELECT_BOX_REWRITE_CANCEL_YESNO:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_NG_EXIT;
          break;
        case 1: //������
          *p_seq  = SEQ_START_BOX_REWRITE_MSG;
          break;
        }
      }
    }
    break;

  //���b�N�m�F
  case SEQ_START_BOX_LOCK_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_10, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_BOX_LOCK_YESNO );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_BOX_LOCK_YESNO:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_BOX_LOCK_YESNO;
    break;

  case SEQ_SELECT_BOX_LOCK_YESNO:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_START_MOVEOUT_CARD;
          break;
        case 1: //������
          *p_seq  = SEQ_START_BOX_LOCK_CANCEL_MSG;
          break;
        }
      }
    }
    break;

  case SEQ_START_BOX_LOCK_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_12, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_BOX_LOCK_CANCEL_YESNO );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_BOX_LOCK_CANCEL_YESNO:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_BOX_LOCK_CANCEL_YESNO;
    break;

  case SEQ_SELECT_BOX_LOCK_CANCEL_YESNO:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_NG_EXIT;
          break;
        case 1: //������
          *p_seq  = SEQ_START_BOX_LOCK_MSG;
          break;
        }
      }
    }
    break;

  case SEQ_START_MOVEOUT_CARD:
    if( Util_PlayerInfo_MoveOut(p_wk) )
    { 
      *p_seq  = SEQ_WAIT_MOVEOUT_CARD;
    }
    break;

  case SEQ_WAIT_MOVEOUT_CARD:
    Util_PlayerInfo_Delete( p_wk );
    *p_seq  = SEQ_START_UPDATE_MSG;
    break;

  //�X�V����
  case SEQ_START_UPDATE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_18, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_SEND_GPF_POKEMON );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_SEND_GPF_POKEMON:
    //GPF�T�[�o�փ|�P������]�����A�o�g���|�P����������
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      GFL_STD_MemClear( p_wk->p_param->p_wifi_sake_data->pokeparty, WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE );
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_POKEPARTY, p_wk->p_param->p_wifi_sake_data->pokeparty );
      *p_seq  = SEQ_WAIT_GPF_POKEMON;
    }
    break;
    
  case SEQ_WAIT_GPF_POKEMON:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_SEND_GPF_CUPSTATUS;
      }

      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        ///	�I��؃`�F�b�N�ŃG���[���N��������K�����O�C��������Ȃ���
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

  case SEQ_SEND_GPF_CUPSTATUS:
    //GPF�T�[�o�֑��J�Ï󋵂�]������
    { 
      DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA data;
      GFL_STD_MemClear( &data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      data.GPFEntryFlg      = DREAM_WORLD_ENTRYFLAG_DS_WRITE;
      data.WifiMatchUpState = DREAM_WORLD_MATCHUP_SIGNUP;
      p_wk->p_param->p_gpf_data->WifiMatchUpState = DREAM_WORLD_MATCHUP_SIGNUP;
      WIFIBATTLEMATCH_NET_StartSendGpfData( p_wk->p_net, &data, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_seq  = SEQ_WAIT_GPF_CUPSTATUS;
    }
    break;

  case SEQ_WAIT_GPF_CUPSTATUS:
    { 
      WIFIBATTLEMATCH_SEND_GPFDATA_RET ret;
      
      ret = WIFIBATTLEMATCH_NET_WaitSendGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS )
      { 
        *p_seq  = SEQ_START_SAVE_MSG;
      }
      else if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      if( ret != WIFIBATTLEMATCH_SEND_GPFDATA_RET_UPDATE )
      {
        ///	�I��؃`�F�b�N�ŃG���[���N��������K�����O�C��������Ȃ���
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_08, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE_UPDATE );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_SAVE_UPDATE:
    { 
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
      REGULATION_VIEWDATA *p_view_sv  = RegulationSaveData_GetRegulationView( p_reg_sv, REGULATION_CARD_TYPE_WIFI );

      //�f�W�^���I��؏㏑��
      GFL_STD_MemCopy( &p_wk->recv_card, p_wk->p_reg, sizeof(REGULATION_CARDDATA) );

      //���b�N����
      BATTLE_BOX_SAVE_OffLockFlg( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI );

      //�T�C���A�b�v��Ԃ�
      Regulation_SetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_SIGNUP );

      //���т��N���A
      RNDMATCH_SetParam( p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_RATE, 0 );
      RNDMATCH_SetParam( p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_WIN, 0 );
      RNDMATCH_SetParam( p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_LOSE, 0 );

      //�V�K���Ȃ̂œo�^�|�P���N���A����
      RegulationView_Init( p_view_sv );

      GAMEDATA_SaveAsyncStart( p_param->p_param->p_game_data );
      *p_seq  = SEQ_WAIT_SAVE_UPDATE;
    }
    break;

  case SEQ_WAIT_SAVE_UPDATE:
    {
      SAVE_RESULT ret;
      ret = GAMEDATA_SaveAsyncMain( p_param->p_param->p_game_data );
      if( ret == SAVE_RESULT_OK )
      {
        *p_seq  = SEQ_REGISTER_EXIT;
      }
      else if( ret == SAVE_RESULT_NG )
      { 
        *p_seq  = SEQ_START_SAVE_NG_MSG;
      }
    }
    break;

    
  case SEQ_START_SAVE_NG_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_14, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_NG_EXIT );
    *p_seq  = SEQ_WAIT_MSG;
    break;

    //-------------------------------------
    /// ��������Ȃ�
    //=====================================
   case SEQ_START_RECV_SAKE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_35, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_RECV_SAKE_DATA );
    *p_seq  = SEQ_WAIT_MSG;
    break; 
  
  case SEQ_RECV_SAKE_DATA:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, WIFIBATTLEMATCH_GDB_MYRECORD, WIFIBATTLEMATCH_GDB_GET_WIFI_SCORE, p_wk->p_param->p_wifi_sake_data );
      *p_seq  = SEQ_WAIT_SAKE_DATA;
    }
    break;
 
  case SEQ_WAIT_SAKE_DATA:
    {
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_GDB_Process( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_CHECK_RATING;
      }

      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        ///	�I��؃`�F�b�N�ŃG���[���N��������K�����O�C��������Ȃ���
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

  case SEQ_CHECK_RATING:
    { 
      u32 my_rate ;

      my_rate = RNDMATCH_GetParam( p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_RATE );
      if( p_wk->p_param->p_wifi_sake_data->rate != my_rate )
      { 
        *p_seq  = SEQ_START_RENEWAL_MSG;
      }
      else
      { 
        *p_seq  = SEQ_NG_EXIT;
      }
    }
    break;

  case SEQ_START_RENEWAL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_08, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE_RENEWAL );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_SAVE_RENEWAL:
    { 
      //Regulation_SetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_END );

      //�Ō�̐��т𔽉f
      Util_SaveScore( p_wk, p_wk->p_param->p_wifi_sake_data );

      GAMEDATA_SaveAsyncStart( p_param->p_param->p_game_data );
      *p_seq  = SEQ_WAIT_SAVE_RENEWAL;
    }
    break;

  case SEQ_WAIT_SAVE_RENEWAL:
    {
      SAVE_RESULT ret = GAMEDATA_SaveAsyncMain( p_param->p_param->p_game_data );
      if( ret == SAVE_RESULT_OK )
      {
        *p_seq  = SEQ_NG_EXIT;
      }
    }
    break;

  case SEQ_REGISTER_EXIT:  //�I��؃`�F�b�N���P�ɂ��ďI��(�Ǝd�l���ɏ����Ă���Ƃ���)
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Register );
    break;

  case SEQ_ALREADY_EXIT:   //�I��؃`�F�b�N���Q�ɂ��ďI��(�Ǝd�l���ɏ����Ă���Ƃ���)
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
    break;

  case SEQ_NG_EXIT:        //�I��؃`�F�b�N���O�ɂ��ďI��(�Ǝd�l���ɏ����Ă���Ƃ���)
    p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextEnd );
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
 *	@brief  WIFI���  �o�^����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Register( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_DRAW_PLAYERINFO,
    SEQ_WAIT_DRAW_PLAYERINFO,
    SEQ_START_REGISTER_POKE_MSG,
    SEQ_START_REGISTER_POKE_LIST,
    SEQ_SELECT_REGISTER_POKE_LIST,
    SEQ_START_REGISTER_POKE_CANCEL_MSG,
    SEQ_START_REGISTER_POKE_CANCEL_LIST,
    SEQ_SELECT_REGISTER_POKE_CANCEL_LIST,
    SEQ_START_REGISTER_CUP_CANCEL_MSG,
    SEQ_START_REGISTER_CUP_CANCEL_LIST,
    SEQ_SELECT_REGISTER_CUP_CANCEL_LIST,
    SEQ_WAIT_MOVEOUT_BTLBOX_END,
    
    SEQ_CHECK_REGULATION,
    SEQ_START_NG_REG_MSG,
    SEQ_START_EMPTY_BOX_MSG,
    SEQ_WAIT_MOVEOUT_BTLBOX,
    SEQ_START_SEND_DATA_MSG,

    SEQ_START_SEND_CHECK_DIRTY_POKE,
    SEQ_WAIT_SEND_CHECK_DIRTY_POKE,
    SEQ_START_DIRTY_POKE_MSG,
    SEQ_START_SEND_SAKE_POKE,
    SEQ_WAIT_SEND_SAKE_POKE,
    SEQ_LOCK_POKE,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_SEND_GPF_CUPSTATUS,
    SEQ_WAIT_GPF_CUPSTATUS,
    SEQ_START_OK_REGISTER_MSG,

    SEQ_NEXT,
    SEQ_DISCONNECT_END,
    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_PLAYERINFO:
    Util_PlayerInfo_Create( p_wk );
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNREGISTER );
    Util_BtlBox_Create( p_wk );
    *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    {
      BOOL ret = TRUE;
      ret &= Util_PlayerInfo_MoveIn( p_wk );
      ret &= Util_BtlBox_MoveIn( p_wk );
      if( ret )
      { 
        *p_seq  = SEQ_START_REGISTER_POKE_MSG;
      }
    }
    break;

  case SEQ_START_REGISTER_POKE_MSG: //�o�g���{�b�N�X�̃|�P�������Ƃ��낭���܂���
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_15, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_REGISTER_POKE_LIST );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_REGISTER_POKE_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_DECIDE ); 
    *p_seq  = SEQ_SELECT_REGISTER_POKE_LIST;
    break;

  case SEQ_SELECT_REGISTER_POKE_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�����Ă�
          *p_seq  = SEQ_CHECK_REGULATION;
          break;

        case 1: //��߂�
          *p_seq  = SEQ_START_REGISTER_POKE_CANCEL_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	�L�����Z��
    //=====================================
  case SEQ_START_REGISTER_POKE_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_16, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_REGISTER_POKE_CANCEL_LIST );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_REGISTER_POKE_CANCEL_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO ); 
    *p_seq  = SEQ_SELECT_REGISTER_POKE_CANCEL_LIST; 
    break;

  case SEQ_SELECT_REGISTER_POKE_CANCEL_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_START_REGISTER_CUP_CANCEL_MSG;
          break;

        case 1: //������
          *p_seq  = SEQ_START_REGISTER_POKE_MSG;
          break;
        }
      }
    }
    break;

  case SEQ_START_REGISTER_CUP_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_17, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_REGISTER_CUP_CANCEL_LIST );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_REGISTER_CUP_CANCEL_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO ); 
    *p_seq  = SEQ_SELECT_REGISTER_CUP_CANCEL_LIST; 
    break;

  case SEQ_SELECT_REGISTER_CUP_CANCEL_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_WAIT_MOVEOUT_BTLBOX_END;
          break;

        case 1: //������
          *p_seq  = SEQ_START_REGISTER_POKE_MSG;
          break;
        }
      }
    }
    break;

  case SEQ_WAIT_MOVEOUT_BTLBOX_END:
    if( Util_BtlBox_MoveOut( p_wk ) )
    { 
      Util_BtlBox_Delete( p_wk );
      *p_seq  = SEQ_DISCONNECT_END;
    }
    break;
    
    //-------------------------------------
    ///	�o�^
    //=====================================
  case SEQ_CHECK_REGULATION:
#ifdef REGULATION_CHECK_ON
    *p_seq  = SEQ_WAIT_MOVEOUT_BTLBOX;
#else
    {
      WIFIBATTLEMATCH_ENEMYDATA *p_my_data = p_param->p_player_data;
      Util_InitMyData( p_my_data, p_wk );

      if( PokeParty_GetPokeCountBattleEnable((POKEPARTY*)p_my_data->pokeparty) == 0 )
      { 
        DEBUG_WIFICUP_Printf( "�o�g���{�b�N�X�ɓ����ĂȂ�����! \n" );
        *p_seq  = SEQ_START_EMPTY_BOX_MSG;
      }
      else
      {
        u32 failed_bit  = 0;
        REGULATION        *p_reg  = RegulationData_GetRegulation( p_wk->p_reg );

        if( POKE_REG_OK == PokeRegulationMatchLookAtPokeParty(p_reg, (POKEPARTY*)p_my_data->pokeparty, &failed_bit) )
        { 
          DEBUG_WIFICUP_Printf( "�o�g���{�b�N�X�̃|�P�������M�����[�V����OK\n" );
          *p_seq  = SEQ_WAIT_MOVEOUT_BTLBOX;
        }
        else
        { 
          DEBUG_WIFICUP_Printf( "�o�g���{�b�N�X�̃|�P�������M�����[�V����NG!! 0x%x \n", failed_bit );
          *p_seq  = SEQ_START_NG_REG_MSG;
        }
      }
    }
#endif
    break;

  case SEQ_START_NG_REG_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_21, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DISCONNECT_END );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_EMPTY_BOX_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_39, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DISCONNECT_END );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_WAIT_MOVEOUT_BTLBOX:
    if( Util_BtlBox_MoveOut( p_wk ) )
    { 
      Util_BtlBox_Delete( p_wk );
      *p_seq  = SEQ_START_SEND_DATA_MSG;
    }
    break;

  case SEQ_START_SEND_DATA_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_18, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SEND_CHECK_DIRTY_POKE );
    *p_seq  = SEQ_WAIT_MSG;
    break;

    //-------------------------------------
    ///	�s���`�F�b�N
    //=====================================
  case SEQ_START_SEND_CHECK_DIRTY_POKE:
    { 
      POKEPARTY *p_my_party = (POKEPARTY*)p_param->p_player_data->pokeparty;

      WIFIBATTLEMATCH_NET_StartEvilCheck( p_wk->p_net, p_my_party, WIFIBATTLEMATCH_NET_EVILCHECK_TYPE_PARTY );
      GFL_STD_MemClear( &p_wk->evilecheck_data, sizeof(WIFIBATTLEMATCH_NET_EVILCHECK_DATA));
      *p_seq = SEQ_WAIT_SEND_CHECK_DIRTY_POKE;
    }
    break;

  case SEQ_WAIT_SEND_CHECK_DIRTY_POKE:
    { 
      WIFIBATTLEMATCH_NET_EVILCHECK_RET ret;
      ret = WIFIBATTLEMATCH_NET_WaitEvilCheck( p_wk->p_net, &p_wk->evilecheck_data );
      if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_SUCCESS )
      { 
        POKEPARTY *p_my_party = (POKEPARTY*)p_param->p_player_data->pokeparty;
        const int poke_max  = PokeParty_GetPokeCount( p_my_party );
        int i;
        int ret = 0;
        for( i = 0; i < poke_max; i++ )
        { 
          if( p_wk->evilecheck_data.poke_result[i] == NHTTP_RAP_EVILCHECK_RESULT_OK )
          { 
            ret++;
          }
        }

        if( p_wk->evilecheck_data.status_code == 0 && ret == poke_max )
        { 
          DEBUG_WIFICUP_Printf( "�s���`�F�b�N�ʉ�\n" );
          *p_seq = SEQ_START_SEND_SAKE_POKE;
        }
        else
        { 
          DEBUG_WIFICUP_Printf( "�s���`�F�b�NNG\n" );
          *p_seq = SEQ_START_DIRTY_POKE_MSG;
        }

      }

      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        *p_seq  = SEQ_START_DRAW_PLAYERINFO;
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_START_DIRTY_POKE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_19, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DISCONNECT_END );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_SEND_SAKE_POKE:
    if( *p_wk->p_param->p_server_time == 0 )
    {
      POKEPARTY *p_my_party = (POKEPARTY*)p_param->p_player_data->pokeparty;
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_POKEPARTY, p_my_party );
      *p_seq  = SEQ_WAIT_SEND_SAKE_POKE;
    }
    break;
    
  case SEQ_WAIT_SEND_SAKE_POKE:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_LOCK_POKE;
      }
      
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
          *p_seq  = SEQ_START_DRAW_PLAYERINFO;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

  case SEQ_LOCK_POKE:
    //�o�g���{�b�N�X�̃��b�N��
    //���M�����[�V�����ւ̌����ړo�^
    //�J�Ï�Ԃ��Q������
    { 
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
      REGULATION_VIEWDATA *p_reg_view = RegulationSaveData_GetRegulationView( p_reg_sv, REGULATION_CARD_TYPE_WIFI );
      POKEPARTY *p_party  = BATTLE_BOX_SAVE_MakePokeParty( p_bbox_save, GFL_HEAP_LOWID(HEAPID_WIFIBATTLEMATCH_SYS) );

      BATTLE_BOX_SAVE_OnLockFlg( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI );

      Regulation_SetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_ENTRY );

      RegulationView_SetEazy( p_reg_view, p_party );

      GFL_HEAP_FreeMemory( p_party );

      *p_seq  = SEQ_START_SAVE;
    }
    break;

  case SEQ_START_SAVE:
    { 
      GAMEDATA_SaveAsyncStart( p_param->p_param->p_game_data );
      *p_seq  = SEQ_WAIT_SAVE;
    }
    break;

  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT ret = GAMEDATA_SaveAsyncMain( p_param->p_param->p_game_data );
      if( ret == SAVE_RESULT_OK )
      { 
        *p_seq  = SEQ_SEND_GPF_CUPSTATUS;
      }
      else if( ret == SAVE_RESULT_NG )
      { 
        *p_seq  = SEQ_DISCONNECT_END;
      }
    }
    break;

  case SEQ_SEND_GPF_CUPSTATUS:
    { 
      DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA data;
      GFL_STD_MemClear( &data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      data.WifiMatchUpState = DREAM_WORLD_MATCHUP_ENTRY;
      p_wk->p_param->p_gpf_data->WifiMatchUpState = DREAM_WORLD_MATCHUP_ENTRY;
      WIFIBATTLEMATCH_NET_StartSendGpfData( p_wk->p_net, &data, HEAPID_WIFIBATTLEMATCH_CORE );
      DEBUG_WIFICUP_Printf("GPF�֑��M %d\n",DREAM_WORLD_MATCHUP_ENTRY);
    }
    *p_seq  = SEQ_WAIT_GPF_CUPSTATUS;
    break;

  case SEQ_WAIT_GPF_CUPSTATUS:
    { 
      WIFIBATTLEMATCH_SEND_GPFDATA_RET ret;
      
      ret = WIFIBATTLEMATCH_NET_WaitSendGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS )
      { 
        *p_seq  = SEQ_START_OK_REGISTER_MSG;
      }
      else if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        *p_seq  = SEQ_START_DRAW_PLAYERINFO;
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_START_OK_REGISTER_MSG:
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_LOCK );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_20, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_NEXT );
    *p_seq  = SEQ_WAIT_MSG; 
    break;

  case SEQ_NEXT:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
    break;

  case SEQ_DISCONNECT_END:
    p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
    break;

  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk ); //�\�񂵂��V�[�P���X�ϐ��ɔ��
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  ���J�n����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_DRAW_PLAYERINFO,
    SEQ_WAIT_DRAW_PLAYERINFO,

    SEQ_START_CUP_MSG,
    SEQ_START_CUP_LIST,
    SEQ_SELECT_CUP_LIST,

    SEQ_NEXT_CUP_END,

    SEQ_START_SUBSEQ_UNREGISTER,
    SEQ_WAIT_SUBSEQ_UNREGISTER,
    SEQ_NEXT_DISCONNECT,

    SEQ_START_EVILCHECK_MSG,
    SEQ_START_SUBSEQ_EVILCHECK,
    SEQ_WAIT_SUBSEQ_EVILCHECK,
    SEQ_NEXT_MATCHING,
    
    SEQ_START_DIRTY_MSG,
    SEQ_DIRTY_END,

    SEQ_WAIT_MSG,
  };
  
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_PLAYERINFO:
    if( p_wk->p_playerinfo == NULL )
    { 
      Util_PlayerInfo_Create( p_wk );
      Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_LOCK );
      *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    }
    else
    { 
      *p_seq  = SEQ_START_CUP_MSG;
    }
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    if( Util_PlayerInfo_MoveIn( p_wk ) )
    { 
      *p_seq  = SEQ_START_CUP_MSG;
    }
    break;

  case SEQ_START_CUP_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_22, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_CUP_LIST );
    *p_seq  = SEQ_WAIT_MSG; 
    break;

  case SEQ_START_CUP_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_CUPMENU );
    *p_seq  = SEQ_SELECT_CUP_LIST;
    break;

  case SEQ_SELECT_CUP_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�������񂷂�@�@�s���`�F�b�N��}�b�`���O��
          *p_seq = SEQ_START_EVILCHECK_MSG;
          break;
        case 1: //���񂩂�������
          *p_seq = SEQ_START_SUBSEQ_UNREGISTER;
          break;
        case 2: //��߂�
          *p_seq = SEQ_NEXT_CUP_END;
          break;
        }
      }
    }
    break;

  case SEQ_NEXT_CUP_END:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupEnd );
    break;
    
  case SEQ_START_SUBSEQ_UNREGISTER:
    Util_SubSeq_Start( p_wk, WbmWifiSubSeq_UnRegister );
    *p_seq  = SEQ_WAIT_SUBSEQ_UNREGISTER;
    break;
  case SEQ_WAIT_SUBSEQ_UNREGISTER:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_FALSE||
          ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_NONE )
      { 
        *p_seq  = SEQ_START_DRAW_PLAYERINFO;
      }
      else if( ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_TRUE )
      { 
        *p_seq  = SEQ_NEXT_DISCONNECT;
      }
      else if( ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_SERVER )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;
  case SEQ_NEXT_DISCONNECT:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
    break;

    //-------------------------------------
    ///	�s���`�F�b�N
    //=====================================
  case SEQ_START_EVILCHECK_MSG:
    Util_InitMyData( p_param->p_player_data, p_wk );

    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SUBSEQ_EVILCHECK );
    *p_seq  = SEQ_WAIT_MSG; 
    break;
  case SEQ_START_SUBSEQ_EVILCHECK:
    Util_SubSeq_Start( p_wk, WbmWifiSubSeq_EvilCheck );
    *p_seq       = SEQ_WAIT_SUBSEQ_EVILCHECK;
    break;

  case SEQ_WAIT_SUBSEQ_EVILCHECK:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_WIFI_SUBSEQ_EVILCHECK_RET_SUCCESS )
      { 
        *p_seq       = SEQ_NEXT_MATCHING;
      }
      else if( ret == WBM_WIFI_SUBSEQ_EVILCHECK_RET_DARTY )
      {   
        //*p_seq       = SEQ_START_DIRTY_MSG;
        *p_seq       = SEQ_NEXT_MATCHING;
      }

      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_NEXT_MATCHING:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Matching );
    break;

  case SEQ_START_DIRTY_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_ERR_00, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    break;

  case SEQ_DIRTY_END:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
    break;

  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk ); //�\�񂵂��V�[�P���X�ϐ��ɔ��
    }
    break;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  �}�b�`���O����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  enum
  { 
    //�����ԃ`�F�b�N
    SEQ_START_MSG,
    SEQ_START_SUBSEQ_CUPDATE,
    SEQ_WAIT_SUBSEQ_CUPDATE,
    SEQ_NEXT_START,
    SEQ_RECV_SAKE_DATA,
    SEQ_WAIT_SAKE_DATA,

    //�T�P�T�[�o�[����X�V���������Ƃ���
    //�J�[�h�X�V����
    SEQ_START_CARD_MOVEOUT,
    SEQ_WAIT_CARD_MOVEOUT,
    SEQ_START_CARD_MOVEIN,
    SEQ_WAIT_CARD_MOVEIN,

    //�}�b�`���O�J�n
    SEQ_START_MATCH_MSG,
    SEQ_START_MATCH,
    SEQ_WAIT_MATCHING,

    SEQ_START_SENDDATA,
    SEQ_WAIT_SENDDATA,
    SEQ_CHECK_YOU_CUPNO,
    SEQ_CHECK_YOU_REGULATION,

    SEQ_START_DOWNLOAD_OTHER_SAKE_POKE,
    SEQ_WAIT_DOWNLOAD_OTHER_SAKE_POKE,

    SEQ_START_SAKE_TIMING,
    SEQ_WAIT_SAKE_TIMING,

    SEQ_CHECK_DIRTY_POKE,
    SEQ_SEND_DIRTY_CHECK_DATA,
    SEQ_RECV_DIRTY_CHECK_DATA,

    SEQ_CHECK_DIRTY,
    SEQ_START_BADWORD,
    SEQ_WAIT_BADWORD,

    SEQ_START_CANCEL_TIMING,
    SEQ_WAIT_CANCEL_TIMING,
    SEQ_START_OK_TIMING,
    SEQ_WAIT_OK_TIMING,

    SEQ_START_OK_MATCHING_MSG,
    SEQ_START_DRAW_MATCHINFO,
    SEQ_WAIT_MOVEIN_MATCHINFO,
    SEQ_WAIT_CNT,
    SEQ_START_SESSION,
    SEQ_WAIT_SESSION,
    SEQ_END_MATCHING_MSG,
    SEQ_END_MATCHING,
    SEQ_ERROR_END,

    //�L�����Z������
    //�����̑ΐ�������ɂ��܂�
    SEQ_START_SELECT_END_MSG,
    SEQ_START_END_LIST,
    SEQ_SELECT_END_LIST,

    //����������Â��܂����H
    SEQ_START_SELECT_CONTINUE_MSG,
    SEQ_START_CONTINUE_LIST,
    SEQ_SELECT_CONTINUE_LIST,

    //�L�����Z�����畜�A���̓����`�F�b�N
    SEQ_START_CUPDATE_MSG_RET,
    SEQ_START_SUBSEQ_CUPDATE_RET,
    SEQ_WAIT_SUBSEQ_CUPDATE_RET,

    SEQ_START_CANCEL,
    SEQ_END_CANCEL,

    SEQ_START_DISCONNECT,
    SEQ_WAIT_DISCONNECT,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;
  UTIL_CANCEL_STATE cancel_state;
  BOOL is_timeout_enable  = FALSE;

  //-------------------------------------
  ///	�L�����Z���V�[�P���X
  //=====================================
  {
    BOOL is_cancel_enable = (SEQ_WAIT_MATCHING <= *p_seq && *p_seq < SEQ_START_CANCEL_TIMING);
    BOOL can_disconnect = !( (SEQ_START_BADWORD <= *p_seq && *p_seq <= SEQ_WAIT_BADWORD ) || ( SEQ_START_DOWNLOAD_OTHER_SAKE_POKE <= *p_seq && *p_seq <= SEQ_WAIT_DOWNLOAD_OTHER_SAKE_POKE ) );
    cancel_state  = Util_Cancel_Seq( p_wk, is_cancel_enable, can_disconnect );

    //�������ؒf
    if( cancel_state == UTIL_CANCEL_STATE_DECIDE )
    {
      *p_seq  = SEQ_START_SELECT_END_MSG;
      return ;
    }
  }

  //-------------------------------------
  ///	�}�b�`���O�V�[�P���X
  //=====================================
  switch( *p_seq )
  { 
    //-------------------------------------
    ///	�����ԃ`�F�b�N
    //=====================================
  case SEQ_START_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SUBSEQ_CUPDATE );
    *p_seq  = SEQ_WAIT_MSG; 
    break;

  case SEQ_START_SUBSEQ_CUPDATE:
    //�G���[
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_NONE:  //����
      Util_SubSeq_Start( p_wk, WbmWifiSubSeq_CheckDate );
      *p_seq  = SEQ_WAIT_SUBSEQ_CUPDATE;
      break;
    }
    break;
  case SEQ_WAIT_SUBSEQ_CUPDATE:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMESAFE )
      { 
        *p_seq  = SEQ_RECV_SAKE_DATA;
      }
      else if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEOVER
          || ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEBEFORE )
      { 
        *p_seq  = SEQ_NEXT_START;
      }
      else if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_NEXT_START:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
    break;

  case SEQ_RECV_SAKE_DATA:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, WIFIBATTLEMATCH_GDB_MYRECORD, WIFIBATTLEMATCH_GDB_GET_WIFI_SCORE, p_wk->p_param->p_wifi_sake_data );
      *p_seq  = SEQ_WAIT_SAKE_DATA;
    }
    break;
    
  case SEQ_WAIT_SAKE_DATA:
    { 
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_GDB_Process( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        WIFIBATTLEMATCH_ENEMYDATA *p_my_data = p_param->p_player_data;


        //�����̃f�[�^�쐬
        *p_wk->p_param->p_server_time  = 0;
        Util_SetMyDataInfo( p_my_data, p_wk );


        //���������Ȃ������풓�ɕۑ�
        p_wk->p_param->p_recv_data->record_save_idx = p_wk->p_param->p_wifi_sake_data->record_save_idx;

#ifdef MYPOKE_SELFCHECK
        {
          POKEPARTY *p_my_party = (POKEPARTY*)p_param->p_player_data->pokeparty;
          {
            int i;
            u8 *p_src = (u8*)p_my_party;
            OS_TFPrintf( 2, "\n\n" );
            for( i = 0; i < PokeParty_GetWorkSize(); i++ )
            { 
              OS_TFPrintf( 2, "%x ", p_src[i] );
            }
          }
          {
            int i;
            u8 *p_src = (u8*)p_my_data->pokeparty;
            OS_TFPrintf( 2, "\n\n" );
            for( i = 0; i < PokeParty_GetWorkSize(); i++ )
            { 
              OS_TFPrintf( 2, "%x ", p_src[i] );
            }
            OS_TFPrintf( 2, "\n\n" );
          }


          if( Util_ComarepPokeParty( (POKEPARTY*)p_wk->p_param->p_wifi_sake_data->pokeparty, p_my_party ) )
          { 
            OS_TFPrintf( 3, "�����̃p�[�e�B�ƃT�P�͈ꏏ\n" );
          }
          else
          { 
            OS_TFPrintf( 3, "�����̃p�[�e�B�ƃT�P���Ⴄ�I�I\n" );
            GF_ASSERT(0);
          }
        }

        //�����Ŏ����̏����͒ʂ�̂��`�F�b�N
        if( Util_VerifyPokeData( p_my_data, (POKEPARTY*)p_wk->p_param->p_player_data->pokeparty, HEAPID_WIFIBATTLEMATCH_CORE ) )
#endif  //MYPOKE_SELFCHECK
        { 
          OS_TFPrintf( 3, "�����̏����͒ʂ�����\n" );

          //�X�V��������
          if( Util_SaveScore( p_wk, p_wk->p_param->p_wifi_sake_data ) )
          {
            *p_seq  = SEQ_START_CARD_MOVEOUT;
          }
          //�X�V�͂Ȃ�����
          else
          {
            *p_seq  = SEQ_START_MATCH_MSG;
          }
        }
      }
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	�J�[�h�X�V����
    //=====================================
  case SEQ_START_CARD_MOVEOUT:
    *p_seq  = SEQ_WAIT_CARD_MOVEOUT;
    break;
  case SEQ_WAIT_CARD_MOVEOUT:
    if( Util_PlayerInfo_MoveOut( p_wk ) )
    {
      Util_PlayerInfo_Delete( p_wk );
      *p_seq  = SEQ_START_CARD_MOVEIN;
    }
    break;
  case SEQ_START_CARD_MOVEIN:
    Util_PlayerInfo_Create( p_wk );
    *p_seq  = SEQ_WAIT_CARD_MOVEIN;
    break;
  case SEQ_WAIT_CARD_MOVEIN:
    if( Util_PlayerInfo_MoveIn(p_wk ) )
    {
      *p_seq  = SEQ_START_MATCH_MSG;
    }
    break;

    //-------------------------------------
    ///	�}�b�`���O�J�n
    //=====================================
  case SEQ_START_MATCH_MSG:
    p_wk->cancel_seq  = 0;
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, TRUE );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_30, WBM_TEXT_TYPE_QUE );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_MATCH );
    break;
  case SEQ_START_MATCH:
    {
      const REGULATION_CARDDATA *cp_reg_card  = p_wk->p_reg;
      WIFIBATTLEMATCH_MATCH_KEY_DATA  data;
      GFL_STD_MemClear( &data, sizeof(WIFIBATTLEMATCH_MATCH_KEY_DATA) );
      data.rate = p_wk->p_param->p_wifi_sake_data->rate;
      data.disconnect = p_wk->p_param->p_wifi_sake_data->disconnect;
      data.cup_no = Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_CUPNO );
      data.btlcnt= p_wk->p_param->p_wifi_sake_data->win + p_wk->p_param->p_wifi_sake_data->lose;
      WIFIBATTLEMATCH_NET_StartMatchMake( p_wk->p_net, WIFIBATTLEMATCH_TYPE_WIFICUP, FALSE, p_param->p_param->btl_rule, &data ); 
      p_wk->other_dirty_cnt = 0;
      p_wk->my_dirty_cnt = 0;
      *p_seq  = SEQ_WAIT_MATCHING;
    }
    break;
  case SEQ_WAIT_MATCHING:
    //�}�b�`���O�G���[
    {
      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    {
      //�}�b�`���O�҂�
      WIFIBATTLEMATCH_NET_MATCHMAKE_STATE state = WIFIBATTLEMATCH_NET_WaitMatchMake( p_wk->p_net );
      //�}�b�`���O�I���҂�
      if( WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_SUCCESS == state ) 
      { 
        *p_seq = SEQ_START_SENDDATA;
      }
      else if( WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_FAILED == state )
      {
        *p_seq = SEQ_START_MATCH;
      }
    }
    break;

  case SEQ_START_SENDDATA:
    if( cancel_state == UTIL_CANCEL_STATE_NONE )
    { 
      if( WIFIBATTLEMATCH_NET_StartEnemyData( p_wk->p_net, p_param->p_player_data ) )
      {  
        *p_seq  = SEQ_WAIT_SENDDATA;
        p_wk->match_timeout = 0;
      }
    }
    break;
  case SEQ_WAIT_SENDDATA:
    {
      WIFIBATTLEMATCH_ENEMYDATA *p_recv;
      if( WIFIBATTLEMATCH_NET_WaitEnemyData( p_wk->p_net, &p_recv ) )
      { 
        u32 dirty ;
        GFL_STD_MemCopy( p_recv, p_param->p_enemy_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
        
        dirty = WIFIBATTLEMATCH_DATA_ModifiEnemyData( p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );  //�s���f�[�^�΍�

        if( dirty == 0 )
        { 
          SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
          WIFI_NEGOTIATION_SAVEDATA* pSV  = WIFI_NEGOTIATION_SV_GetSaveData(p_sv_ctrl);
//          WIFI_NEGOTIATION_SV_SetFriend(pSV, (MYSTATUS*)p_param->p_enemy_data->mystatus );
          //�d�l���Ȃ��Ȃ�܂���
        }

        *p_seq  = SEQ_CHECK_YOU_CUPNO;
      }

      is_timeout_enable = TRUE;
    }
    break;
  case SEQ_CHECK_YOU_CUPNO:
    WIFIBATTLEMATCH_DATA_DebugPrint( p_param->p_player_data );
    WIFIBATTLEMATCH_DATA_DebugPrint( p_param->p_enemy_data );
#ifdef DEBUG_REG_CRC_CHECK_OFF 
    if( p_param->p_player_data->wificup_no == p_param->p_enemy_data->wificup_no )
#else
    if( p_param->p_player_data->wificup_no == p_param->p_enemy_data->wificup_no
        && p_param->p_player_data->reg_crc == p_param->p_enemy_data->reg_crc )
#endif
    { 
      DEBUG_WIFICUP_Printf( "�������I�I\n" );
      *p_seq  = SEQ_CHECK_YOU_REGULATION;
    }
    else
    {
      //���ԍ������ƂȂ��Ă�����}�b�`���O�ɖ߂�
      DEBUG_WIFICUP_Printf( "���ԍ����Ⴄ�̂Ń}�b�`���O�ɖ߂�\n" );
      *p_seq  = SEQ_START_DISCONNECT;
    }
    break;
  case SEQ_CHECK_YOU_REGULATION:
    {
      u32 failed_bit  = 0;
      REGULATION        *p_reg  = RegulationData_GetRegulation( p_wk->p_reg );
      POKEPARTY         *p_party=  (POKEPARTY*)p_param->p_enemy_data->pokeparty;
      //����̃|�P���������M�����[�V�����`�F�b�N�ɂ�����
      if( POKE_REG_OK == PokeRegulationMatchLookAtPokeParty(p_reg, p_party, &failed_bit)  )
      {
        DEBUG_WIFICUP_Printf( "����̃|�P�����̓��M�����[�V����OK\n" );
        *p_seq = SEQ_START_DOWNLOAD_OTHER_SAKE_POKE;
      }
      else
      { 
        p_wk->other_dirty_cnt++;
        DEBUG_WIFICUP_Printf( "!!����̃|�P�����̓��M�����[�V����NG!! 0x%x\n", failed_bit );
        *p_seq = SEQ_SEND_DIRTY_CHECK_DATA;
      }
    }
    break;

  case SEQ_START_DOWNLOAD_OTHER_SAKE_POKE:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, p_wk->p_param->p_enemy_data->sake_recordID, WIFIBATTLEMATCH_GDB_GET_PARTY, p_wk->p_other_party );
      *p_seq  = SEQ_WAIT_DOWNLOAD_OTHER_SAKE_POKE;
    }
    break;
  case SEQ_WAIT_DOWNLOAD_OTHER_SAKE_POKE:
    { 
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_GDB_Process( p_wk->p_net );

      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC2;
        *p_seq  = SEQ_START_SAKE_TIMING;
      }
      
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

  case SEQ_START_SAKE_TIMING:
    //�T�P�̃T�[�o�[�҂������݂��邽�߁A���̂��Ƃ�B�L�����Z���̃^�C���A�E�g�ɉe�����łĂ��܂�
    //���̂��߁A�����ň�U�������Ƃ�A���̂��Ƃ̃^�C���A�E�g���s����悤�ɂ���
    WIFIBATTLEMATCH_NET_StartTiming( p_wk->p_net, WIFIBATTLEMATCH_NET_TIMINGSYNC_MATHING_SAKE );
    *p_seq  = SEQ_WAIT_SAKE_TIMING;
    break;

  case SEQ_WAIT_SAKE_TIMING:
    if( WIFIBATTLEMATCH_NET_WaitTiming( p_wk->p_net ) )
    {
      *p_seq  = SEQ_CHECK_DIRTY_POKE;
    }
    if( GFL_NET_IsInit() )
    { 
      const GFL_NETSTATE_DWCERROR* cp_error  =  GFL_NET_StateGetWifiError();
      if( cp_error->errorUser == ERRORCODE_TIMEOUT )
      { 
        *p_seq  = SEQ_START_CANCEL;
      }
    }

    //�G���[
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;
    }
    break;

  case SEQ_CHECK_DIRTY_POKE:
    //����̃T�[�o�[�|�P�����Ƒ��M����Ă����|�P�����͓��ꂩ�B
    if( !Util_ComarepPokeParty( p_wk->p_other_party, (POKEPARTY*)p_wk->p_param->p_enemy_data->pokeparty ) )
    { 
      DEBUG_WIFICUP_Printf( "!!����̃|�P�����ƃT�[�o�[�|�P�����ႤNG!! \n" );
      p_wk->other_dirty_cnt++;
    }
    else
    { 
      DEBUG_WIFICUP_Printf( "!!����̃|�P�����ƃT�[�o�[�|�P��������v!! \n" );
    }

    //����̃T�[�o�[�|�P�����Ə�������A����f�[�^�ł��邱�Ƃ��ؖ�
    if( !Util_VerifyPokeData( p_wk->p_param->p_enemy_data, (POKEPARTY*)p_wk->p_param->p_enemy_data->pokeparty, HEAPID_WIFIBATTLEMATCH_CORE ) )
    { 
      DEBUG_WIFICUP_Printf( "!!����̃|�P��������NG!! \n" );
      p_wk->other_dirty_cnt++;
    }
    else
    { 
      DEBUG_WIFICUP_Printf( "!!����̃|�P�����������S��v!! \n" );
    }

    *p_seq  = SEQ_SEND_DIRTY_CHECK_DATA;
    break;
  case SEQ_SEND_DIRTY_CHECK_DATA:
    if( cancel_state == UTIL_CANCEL_STATE_NONE )
    {
      if( WIFIBATTLEMATCH_NET_SendDirtyCnt( p_wk->p_net, &p_wk->other_dirty_cnt ) )
      { 
        *p_seq  = SEQ_RECV_DIRTY_CHECK_DATA;
        p_wk->match_timeout = 0;
      }
    }
    break;
  case SEQ_RECV_DIRTY_CHECK_DATA:
    if( WIFIBATTLEMATCH_NET_RecvDirtyCnt( p_wk->p_net, &p_wk->my_dirty_cnt ) )
    { 
      DEBUG_WIFICUP_Printf( "�s���J�E���^�@����%d����%d\n", p_wk->my_dirty_cnt, p_wk->other_dirty_cnt );
      *p_seq  = SEQ_CHECK_DIRTY;
    }
    else
    {
      is_timeout_enable = TRUE;
    }
    break;

  case SEQ_CHECK_DIRTY:
    if( p_wk->my_dirty_cnt == 0 && p_wk->other_dirty_cnt == 0 )  //�s���J�E���^�`�F�b�N
    { 
      *p_seq  = SEQ_START_BADWORD;
    }
    else
    { 
      //�s����������}�b�`���O�ɖ߂�
      *p_seq  = SEQ_START_DISCONNECT;
    }
    break;

  case SEQ_START_BADWORD:
    WIFIBATTLEMATCH_NET_StartBadWord( p_wk->p_net, (MYSTATUS*)p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );
    *p_seq  = SEQ_WAIT_BADWORD;
    break;
  case SEQ_WAIT_BADWORD:
    { 
      BOOL ret;
      BOOL is_badword;
      
      ret = WIFIBATTLEMATCH_NET_WaitBadWord( p_wk->p_net, &is_badword );
      if( ret )
      { 
        *p_wk->p_param->p_is_dirty_name  = is_badword;
        *p_seq  = SEQ_START_CANCEL_TIMING;
      }

      { 
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

  case SEQ_START_CANCEL_TIMING:
    //�L�����Z����Ԃ̂Ƃ��̓^�C�~���O�Ƃ�Ȃ�
    if( cancel_state == UTIL_CANCEL_STATE_NONE )
    {
      WIFIBATTLEMATCH_NET_StartTiming( p_wk->p_net, WIFIBATTLEMATCH_NET_TIMINGSYNC_MATHING_CANCEL );
      p_wk->match_timeout = 0;
      *p_seq  = SEQ_WAIT_CANCEL_TIMING;
    }
    break;

  case SEQ_WAIT_CANCEL_TIMING:
    if( WIFIBATTLEMATCH_NET_RecvMatchCancel( p_wk->p_net ) == FALSE )
    {
      if( WIFIBATTLEMATCH_NET_WaitTiming( p_wk->p_net ) )
      {
        *p_seq  = SEQ_START_OK_TIMING;
      }
    }
    is_timeout_enable = TRUE;
    break;

  case SEQ_START_OK_TIMING:
    WIFIBATTLEMATCH_NET_StartTiming( p_wk->p_net, WIFIBATTLEMATCH_NET_TIMINGSYNC_MATHING_OK );
    *p_seq  = SEQ_WAIT_OK_TIMING;
    break;

  case SEQ_WAIT_OK_TIMING:
    if( WIFIBATTLEMATCH_NET_WaitTiming( p_wk->p_net ) )
    {
      *p_seq  = SEQ_START_OK_MATCHING_MSG;
    }
    break;

  case SEQ_START_OK_MATCHING_MSG:

    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );

    PMSND_PlaySE( WBM_SND_SE_MATCHING_OK );

    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_32, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_DRAW_MATCHINFO ); 
    p_wk->cnt = 0;
    break;
  case SEQ_START_DRAW_MATCHINFO:
    if( p_wk->cnt++ > MATCHING_MSG_WAIT_SYNC )
    { 
      p_wk->cnt      = 0;
      GFL_BG_SetVisible( BG_FRAME_M_TEXT, FALSE );
      //�ΐ�ҏ��\��
      Util_MatchInfo_Create( p_wk, p_param->p_enemy_data );
      *p_seq  = SEQ_WAIT_MOVEIN_MATCHINFO;
    }
    break;
  case SEQ_WAIT_MOVEIN_MATCHINFO:
    if( Util_MatchInfo_Main( p_wk ) )
    {
      *p_seq  = SEQ_WAIT_CNT;
    }
    break;
  case SEQ_WAIT_CNT:
    if( p_wk->cnt++ > ENEMYDATA_WAIT_SYNC )
    { 
      p_wk->cnt = 0;
      *p_seq  = SEQ_START_SESSION;
    }
    break;

  case SEQ_START_SESSION:
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_SC, WBM_SC_HEAP_SIZE );
    DWC_RAPCOMMON_SetSubHeapID( DWC_ALLOCTYPE_GS, WBM_SC_HEAP_SIZE, HEAPID_WIFIBATTLEMATCH_SC );

    WIFIBATTLEMATCH_SC_StartReport( p_wk->p_net, WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_AFTER, WIFIBATTLEMATCH_TYPE_WIFICUP, 0, NULL, FALSE );
    p_wk->is_send_report  = FALSE;
    *p_seq  = SEQ_WAIT_SESSION;
    break;

  case SEQ_WAIT_SESSION:
    {
      WIFIBATTLEMATCH_NET_SC_STATE  state = WIFIBATTLEMATCH_SC_ProcessReport(p_wk->p_net, &p_wk->is_send_report );
      if( state == WIFIBATTLEMATCH_NET_SC_STATE_SUCCESS )
      { 
        *p_seq  = SEQ_END_MATCHING_MSG;
      }
      
      if( state != WIFIBATTLEMATCH_NET_SC_STATE_UPDATE )
      {
        //�����ŃG���[���N�������ꍇ�A���|�[�g�𑗐M���Ă���ΐؒf�J�E���^�[���������Ă��܂��̂Ő퓬��ցA���|�[�g�𑗐M���Ă��Ȃ���΁A�^���ւ���
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:     //�߂�
          WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
          /* fallthrough */
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
          DWC_RAPCOMMON_ResetSubHeapID();
          GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SC );

          GFL_BG_SetVisible( BG_FRAME_M_TEXT, TRUE );
          Util_Matchinfo_Clear( p_wk );

          *p_seq  = SEQ_ERROR_END;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          DWC_RAPCOMMON_ResetSubHeapID();
          GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SC );

          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

  case SEQ_END_MATCHING_MSG:

    DWC_RAPCOMMON_ResetSubHeapID();
    GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SC );

    GFL_BG_SetVisible( BG_FRAME_M_TEXT, TRUE );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_012, WBM_TEXT_TYPE_WAIT );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_END_MATCHING );
    break;


  case SEQ_END_MATCHING:
    if( p_wk->cnt++ > SELECTPOKE_MSG_WAIT_SYNC )
    { 
      p_wk->cnt = 0;
      p_param->result = WIFIBATTLEMATCH_CORE_RESULT_NEXT_BATTLE;
      WBM_SEQ_End( p_seqwk );
    }
    break;

  case SEQ_ERROR_END:
    WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
    p_param->mode = WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;

    if( p_wk->is_send_report )
    {
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_EndBattle );
    }
    else
    {
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
    }
    break;

    //-------------------------------------
    ///	�����̑ΐ�������ɂ��܂�
    //=====================================
  case SEQ_START_SELECT_END_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_23, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_END_LIST ); 
    break;
  case SEQ_START_END_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_END_LIST;
    break;
  case SEQ_SELECT_END_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextCheckDate );
          break;
        case 1: //������
          *p_seq  = SEQ_START_SELECT_CONTINUE_MSG;
          break;
        }
      }
    }
    break;

    //����������Â��܂����H
  case SEQ_START_SELECT_CONTINUE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_015, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_CONTINUE_LIST );
    break;
  case SEQ_START_CONTINUE_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_CONTINUE_LIST;
    break;
  case SEQ_SELECT_CONTINUE_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_START_CUPDATE_MSG_RET;
          break;
        case 1: //������
          *p_seq  = SEQ_START_SELECT_END_MSG;
          break;
        }
      }
    }
    break;

    //�L�����Z�����畜�A���̓����`�F�b�N
  case SEQ_START_CUPDATE_MSG_RET:
//    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
//    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SUBSEQ_CUPDATE_RET );
//    *p_seq  = SEQ_WAIT_MSG; 
    *p_seq = SEQ_START_SUBSEQ_CUPDATE_RET;
    break;
  case SEQ_START_SUBSEQ_CUPDATE_RET:
    //�G���[
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;
    case WIFIBATTLEMATCH_NET_ERROR_NONE:
      Util_SubSeq_Start( p_wk, WbmWifiSubSeq_CheckDate );
      *p_seq  = SEQ_WAIT_SUBSEQ_CUPDATE_RET;
      break;
    }
    break;
  case SEQ_WAIT_SUBSEQ_CUPDATE_RET:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMESAFE )
      { 
        *p_seq  = SEQ_START_MATCH_MSG;
      }
      else if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEOVER
          || ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEBEFORE )
      { 
        *p_seq  = SEQ_NEXT_START;
      }
      else if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_START_CANCEL:
    if( WIFIBATTLEMATCH_NET_SendMatchCancel( p_wk->p_net ) )
    {
      *p_seq  = SEQ_END_CANCEL;
    }
    break;

  case SEQ_END_CANCEL:
    WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
    *p_seq  = SEQ_START_MATCH_MSG;
    break;

  case SEQ_START_DISCONNECT:
    WIFIBATTLEMATCH_NET_StartDisConnect( p_wk->p_net );
    *p_seq  = SEQ_WAIT_DISCONNECT;
    p_wk->match_timeout = 0;
    break;

  case SEQ_WAIT_DISCONNECT:
    is_timeout_enable = TRUE;
    if( WIFIBATTLEMATCH_NET_WaitDisConnect( p_wk->p_net ) )
    {
      *p_seq = SEQ_START_MATCH;
    }
    break;

    //-------------------------------------
    ///	����
    //=====================================
  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }

  //-------------------------------------
  ///	�^�C���A�E�g�`�F�b�N
  //=====================================
  {
    if( is_timeout_enable )
    {
      BOOL is_disconnect  = FALSE;
      //���肪�ؒf
      if( p_wk->match_timeout++ > MATCHING_TIMEOUT_SYNC )
      {
        OS_TPrintf( "�^�C���A�E�g�I seq=%d\n", *p_seq );
        p_wk->match_timeout = 0;
        is_disconnect  = TRUE;
      }

      //���肪�ؒf
      if( WIFIBATTLEMATCH_NET_RecvMatchCancel( p_wk->p_net ) )
      {
        OS_TPrintf( "����̐ؒf�����m�I seq=%d\n", *p_seq );
        is_disconnect  = TRUE;
      }

      if( is_disconnect )
      {
        *p_seq  = SEQ_START_CANCEL;
      }


      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  �퓬�㏈��
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_SC_HEAP_INIT,
    SEQ_START_NET_MSG,
    SEQ_START_REPORT_ATLAS,
    SEQ_WAIT_REPORT_ATLAS,

    SEQ_SC_HEAP_EXIT,

    SEQ_INIT_DISCONNECT,
    SEQ_WAIT_DISCONNECT,
    SEQ_START_SAKE_RECORD,
    SEQ_WAIT_SAKE_RECORD,

    SEQ_START_RESULT_MSG,
    SEQ_END,

    //�����ԃ`�F�b�N
    SEQ_START_SUBSEQ_CUPDATE,
    SEQ_WAIT_SUBSEQ_CUPDATE,

    //����
    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_SC_HEAP_INIT:
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_SC, WBM_SC_HEAP_SIZE );
    DWC_RAPCOMMON_SetSubHeapID( DWC_ALLOCTYPE_GS, WBM_SC_HEAP_SIZE, HEAPID_WIFIBATTLEMATCH_SC );
    *p_seq  = SEQ_START_NET_MSG;
    break;
  case SEQ_START_NET_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_18, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_REPORT_ATLAS );
    break;

  case SEQ_START_REPORT_ATLAS:
    {
      BOOL is_error = p_param->mode == WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;
      WIFIBATTLEMATCH_SC_StartReport( p_wk->p_net, WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_SCORE, WIFIBATTLEMATCH_TYPE_WIFICUP, p_param->p_param->btl_rule, p_param->cp_btl_score, is_error );
    }
    *p_seq = SEQ_WAIT_REPORT_ATLAS;
    break;
  case SEQ_WAIT_REPORT_ATLAS:
    { 
      WIFIBATTLEMATCH_NET_SC_STATE  state = WIFIBATTLEMATCH_SC_ProcessReport(p_wk->p_net, NULL );
      if( state == WIFIBATTLEMATCH_NET_SC_STATE_SUCCESS )
      { 
        //����̕s���t���O���󂯎��
        ((BATTLEMATCH_BATTLE_SCORE *)(p_param->cp_btl_score))->is_other_dirty = WIFIBATTLEMATCH_NET_SC_GetDirtyFlag( p_wk->p_net );
        *p_seq = SEQ_SC_HEAP_EXIT;
      }
      
      if( state != WIFIBATTLEMATCH_NET_SC_STATE_UPDATE  )
      {
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:     //�߂�
          WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
          /* fallthrough */
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
          DWC_RAPCOMMON_ResetSubHeapID();
          GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SC );
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          DWC_RAPCOMMON_ResetSubHeapID();
          GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SC );
          break;
        }
      }
    }
    break;

  case SEQ_SC_HEAP_EXIT:
    DWC_RAPCOMMON_ResetSubHeapID();
    GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SC );

    //����ɐؒf����Ă�����A�^����΂�
    if( p_param->mode == WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR )
    { 
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_EndRec );
    }
    else
    { 
      *p_seq = SEQ_INIT_DISCONNECT;
    }
    break;

    //-------------------------------------
    ///	�ؒf����
    //=====================================
  case SEQ_INIT_DISCONNECT:
    //�����܂Őؒf�����Ȃ��ꍇ�͑����m�荇���ɓo�^
    {           
      SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      ETC_SAVE_WORK *p_etc  = SaveData_GetEtc( p_sv_ctrl );
      WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data = p_param->p_enemy_data;
      EtcSave_SetAcquaintance( p_etc, MyStatus_GetID( (MYSTATUS*)p_enemy_data->mystatus ) );
    }
    WIFIBATTLEMATCH_NET_StartDisConnect( p_wk->p_net );
    *p_seq = SEQ_WAIT_DISCONNECT;
    break;

  case SEQ_WAIT_DISCONNECT:
    if( WIFIBATTLEMATCH_NET_WaitDisConnect( p_wk->p_net ) )
    { 
      *p_seq = SEQ_START_SAKE_RECORD;
    }

    //�G���[
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
      *p_seq  = SEQ_START_SAKE_RECORD;
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;
    }
    break;
    
  case SEQ_START_SAKE_RECORD:
    //����������
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_RND_RECORD_DATA data;
      GFL_STD_MemClear( &data, sizeof(WIFIBATTLEMATCH_GDB_RND_RECORD_DATA) );
      data.record_data  = *p_wk->p_param->p_record_data;
      data.save_idx     = p_wk->p_param->p_recv_data->record_save_idx;
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_RECORD, &data );
      *p_seq  = SEQ_WAIT_SAKE_RECORD;
    }
    break;

  case SEQ_WAIT_SAKE_RECORD:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq = SEQ_START_RESULT_MSG;
      }

      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      { 
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    } 
    break;

  case SEQ_START_RESULT_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_33, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_END );
    break;
    
  case SEQ_END:
    p_param->result = WIFIBATTLEMATCH_CORE_RESULT_NEXT_REC;
    WBM_SEQ_End( p_seqwk ); 
    break;

    //-------------------------------------
    ///	����
    //=====================================
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
 *	@brief  WIFI���  �^��I����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_EndRec( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_NET_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,

    //�����ԃ`�F�b�N
    SEQ_START_SUBSEQ_CUPDATE,
    SEQ_WAIT_SUBSEQ_CUPDATE,

    //����
    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_NET_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    break;

  case SEQ_START_SAVE:
    { 
      Util_SaveScore( p_wk, p_wk->p_param->p_wifi_sake_data );
    }
    GAMEDATA_SaveAsyncStart(p_param->p_param->p_game_data);
    *p_seq  = SEQ_WAIT_SAVE;
    break;

  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT result;
      result = GAMEDATA_SaveAsyncMain(p_param->p_param->p_game_data);
      if( result == SAVE_RESULT_OK )
      { 
        *p_seq  = SEQ_START_SUBSEQ_CUPDATE;
      }
      else if(result == SAVE_RESULT_NG)
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
      }
    }
    break;

  case SEQ_START_SUBSEQ_CUPDATE:
    //�G���[
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_NONE:
      Util_SubSeq_Start( p_wk, WbmWifiSubSeq_CheckDate );
      *p_seq  = SEQ_WAIT_SUBSEQ_CUPDATE;
      break;
    }
    break;
  case SEQ_WAIT_SUBSEQ_CUPDATE:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMESAFE )
      { 
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
      }
      else if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEOVER ||
          ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEBEFORE )
      { 
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
      }
      else if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

    //-------------------------------------
    ///	����
    //=====================================
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
 *	@brief  WIFI���  ���s�m�F����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_CupContinue( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK_ERR,

    SEQ_START_SELECT_CONTINUE_MSG,
    SEQ_START_SELECT_CONTINUE,
    SEQ_WAIT_SELECT_CONTINUE,

    SEQ_START_SUBSEQ_UNREGISTER,
    SEQ_WAIT_SUBSEQ_UNREGISTER,
    SEQ_NEXT_DISCONNECT,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECK_ERR:
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:     //�߂�
    case WIFIBATTLEMATCH_NET_ERROR_NONE:
      *p_seq  = SEQ_START_SELECT_CONTINUE_MSG;
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;
    }
    break;

    //-------------------------------------
    ///	���s�m�F
    //=====================================
  case SEQ_START_SELECT_CONTINUE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_37, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SELECT_CONTINUE );
    break;
  case SEQ_START_SELECT_CONTINUE:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_CUPMENU_CONT );
    *p_seq     = SEQ_WAIT_SELECT_CONTINUE;
    break;
  case SEQ_WAIT_SELECT_CONTINUE:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�ΐ킷��
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Matching );
          break;
        case 1://�Q������������
          *p_seq  = SEQ_START_SUBSEQ_UNREGISTER;
          break;
        case 2://��߂�
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupEnd );
          break;
        }
      }
    }
    break;

  case SEQ_START_SUBSEQ_UNREGISTER:
    Util_SubSeq_Start( p_wk, WbmWifiSubSeq_UnRegister );
    *p_seq  = SEQ_WAIT_SUBSEQ_UNREGISTER;
    break;
  case SEQ_WAIT_SUBSEQ_UNREGISTER:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_FALSE||
          ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_NONE )
      { 
        *p_seq  = SEQ_START_SELECT_CONTINUE_MSG;
      }
      else if( ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_TRUE )
      { 
        *p_seq  = SEQ_NEXT_DISCONNECT;
      }

      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_NEXT_DISCONNECT:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
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
 *	@brief  WIFI���  ���I������
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_END_MSG,
    SEQ_START_END_LIST,
    SEQ_SELECT_END_LIST,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_END_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_23, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_END_LIST );
    break;
    
  case SEQ_START_END_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_END_LIST;
    break;

  case SEQ_SELECT_END_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextCheckDate );
          break;
        case 1: //������
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
          break;
        }
      }
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
 *	@brief  WIFI���  �ؒf����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_DisConnextSendTime( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  enum
  { 
    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_START_LOGINTIME_MSG,
    SEQ_SEND_SAKE_LOGINTIME,
    SEQ_WAIT_SAKE_LOGINTIME,
    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    *p_seq  = SEQ_WAIT_MSG;
    break;
  case SEQ_START_SAVE:
    GAMEDATA_SaveAsyncStart( p_param->p_param->p_game_data );
    *p_seq  = SEQ_WAIT_SAVE;
    break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT ret;
      ret = GAMEDATA_SaveAsyncMain( p_param->p_param->p_game_data );
      if( ret == SAVE_RESULT_OK )
      {
        *p_seq  = SEQ_START_LOGINTIME_MSG;
      }
    }
    break;
  case SEQ_START_LOGINTIME_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_013, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_SEND_SAKE_LOGINTIME );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_SEND_SAKE_LOGINTIME:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_MYINFO, NULL );
      *p_seq  = SEQ_WAIT_SAKE_LOGINTIME;
    }
    break;

  case SEQ_WAIT_SAKE_LOGINTIME:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_END;

        //�G���[
        WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE );
      }
    }
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextEnd );
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
 *	@brief  WIFI���  �����`�F�b�N�����ďI��
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_DisConnextCheckDate( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_SUBSEQ_CUPDATE,
    SEQ_WAIT_SUBSEQ_CUPDATE,
    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_SUBSEQ_CUPDATE:
    //�G���[
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;
    case WIFIBATTLEMATCH_NET_ERROR_NONE:
      Util_SubSeq_Start( p_wk, WbmWifiSubSeq_CheckDate );
      *p_seq  = SEQ_WAIT_SUBSEQ_CUPDATE;
      break;
    }
    break;

  case SEQ_WAIT_SUBSEQ_CUPDATE:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret != WBM_WIFI_SUBSEQ_RET_NONE )
      { 
        *p_seq  = SEQ_END;
      }
      
      if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
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
 *	@brief  WIFI���  �I������
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_DisConnextEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  //�^�C�g���ֈ�C�ɖ߂�ꍇ���Ȃ��Ȃ�܂���
 //   p_param->result = WIFIBATTLEMATCH_CORE_RESULT_END_WIFICUP;

  WBM_SEQ_End( p_seqwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  �G���[����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Err_ReturnLogin( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  p_param->result = WIFIBATTLEMATCH_CORE_RESULT_ERR_NEXT_LOGIN;
  
  if( p_wk->p_net )
  { 
    WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );
    p_wk->p_net = NULL;
  }

  NetErr_ExitNetSystem();

  WBM_SEQ_End( p_seqwk );
}
//=============================================================================
/**
///	�T�u�V�[�P���X�֐��i���C���V�[�P���X��œ����V�[�P���X�����j
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  �����Ԃ��`�F�b�N
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSubSeq_CheckDate( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK_CUP_STATUS,

    SEQ_CHECK_DATE,

    SEQ_START_DATEBEFORE_MSG,
    SEQ_START_DATE_MSG,
    SEQ_CHECK_STATUS,
    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_CHECK_LOCK,
    SEQ_START_UNLOCK_MSG,
    SEQ_START_SEND_MSG,
    SEQ_SEND_GPF_CUPSTATUS,
    SEQ_WAIT_GPF_CUPSTATUS,
    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECK_CUP_STATUS:
    *p_seq  = SEQ_CHECK_DATE;
    break;

  case SEQ_CHECK_DATE:
    { 
      RTCDate start_date;
      RTCDate end_date;
      RTCDate now_date;
      RTCTime time;
      BOOL    ret;
      s32 start;
      s32 now;
      s32 end;

      REGULATION_CARDDATA *p_reg = p_wk->p_reg;
      ret = DWC_TOOL_GetLocalDateTime( &now_date, &time );

#ifdef PM_DEBUG
      if( ((*DEBUGWIN_SERVERTIME_GetFlag()) && (*DEBUGWIN_BATTLE_GetFlag()))
        || (*DEBUGWIN_SERVERTIME_USER_GetFlag()) )
      {
        now_date.day  += 1;
      }
#endif //PM_DEBUG

      start_date.year  = Regulation_GetCardParam( p_reg, REGULATION_CARD_START_YEAR );
      start_date.month = Regulation_GetCardParam( p_reg, REGULATION_CARD_START_MONTH );
      start_date.day   = Regulation_GetCardParam( p_reg, REGULATION_CARD_START_DAY );
      start_date.week  = RTC_WEEK_SUNDAY; //RTC_ConvertDateToDay�֐��̂Ȃ��Ŕ͈̓`�F�b�N���Ă�̂ł��܂����߂ɓK���ȗj���������

      end_date.year    = Regulation_GetCardParam( p_reg, REGULATION_CARD_END_YEAR );
      end_date.month   = Regulation_GetCardParam( p_reg, REGULATION_CARD_END_MONTH );
      end_date.day     = Regulation_GetCardParam( p_reg, REGULATION_CARD_END_DAY );
      end_date.week    = RTC_WEEK_SUNDAY; //RTC_ConvertDateToDay�֐��̂Ȃ��Ŕ͈̓`�F�b�N���Ă�̂ł��܂����߂ɓK���ȗj���������

      start = RTC_ConvertDateToDay( &start_date );
      end   = RTC_ConvertDateToDay( &end_date );
      now   = RTC_ConvertDateToDay( &now_date );

      OS_TFPrintf( 3, "�J�Ê��ԃ`�F�b�N �J�n%d.%d.%d[%d] ����%d.%d.%d[%d] �I��%d.%d.%d[%d]\n",
                        start_date.year, start_date.month, start_date.day, start,
                        now_date.year, now_date.month, now_date.day, now,
                        end_date.year, end_date.month, end_date.day, end );

      if( ret && ( now < start ) )
      { 
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEBEFORE;
        *p_seq  = SEQ_START_DATEBEFORE_MSG;
      }
      else if( ret && ( start <= now && now <= end ) )
      {
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMESAFE;
        *p_seq  = SEQ_END;
      }
      else
      { 
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEOVER;
        *p_seq  = SEQ_START_DATE_MSG;
      }
    }
    break;

  case SEQ_START_DATEBEFORE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_38, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_END );
    break;

  case SEQ_START_DATE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_25, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_CHECK_STATUS );
    break;


  case SEQ_CHECK_STATUS:
    {
      if( Regulation_GetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS ) == DREAM_WORLD_MATCHUP_SIGNUP
        || Regulation_GetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS ) == DREAM_WORLD_MATCHUP_ENTRY )
      {
        *p_seq  = SEQ_START_SAVE_MSG;
      }
      else
      { 
        *p_seq  = SEQ_END;
      }
    }
    break;

  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    break;

  case SEQ_START_SAVE:
    { 
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      BATTLE_BOX_SAVE_OffLockFlg( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI );

      //���I����Ԃɂ��ăZ�[�u
      Regulation_SetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_END );

      GAMEDATA_SaveAsyncStart( p_param->p_param->p_game_data );
      *p_seq  = SEQ_WAIT_SAVE;
    }
    break;

  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT ret = GAMEDATA_SaveAsyncMain( p_param->p_param->p_game_data );
      if( ret == SAVE_RESULT_OK )
      { 
        *p_seq  = SEQ_CHECK_LOCK;
      }
    }
    break;


  case SEQ_CHECK_LOCK:
    {
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      if( BATTLE_BOX_SAVE_GetLockType( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI ) )
      {
        *p_seq  = SEQ_START_UNLOCK_MSG;
      }
      else
      { 
        *p_seq  = SEQ_START_SEND_MSG;
      }
    }
    break;

  case SEQ_START_UNLOCK_MSG:
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_26, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SEND_MSG );
    break;
    
  case SEQ_START_SEND_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_18, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_SEND_GPF_CUPSTATUS );
    break;

  case SEQ_SEND_GPF_CUPSTATUS:
    { 
      DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA data;
      GFL_STD_MemClear( &data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      data.WifiMatchUpState = DREAM_WORLD_MATCHUP_END;
      p_wk->p_param->p_gpf_data->WifiMatchUpState = DREAM_WORLD_MATCHUP_END;
      WIFIBATTLEMATCH_NET_StartSendGpfData( p_wk->p_net, &data, HEAPID_WIFIBATTLEMATCH_CORE ); 
      *p_seq  = SEQ_WAIT_GPF_CUPSTATUS;
    }
    break;
    
 case SEQ_WAIT_GPF_CUPSTATUS:
    { 
      WIFIBATTLEMATCH_SEND_GPFDATA_RET ret;
      
      ret = WIFIBATTLEMATCH_NET_WaitSendGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS )
      { 
        *p_seq  = SEQ_END;
      }
      else if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY )
      {
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER;
        *p_seq  = SEQ_END;
      }

      //�����̓T�u�V�[�P���X�Ȃ̂ŃG���[�����͂��̏�ōs��
    }
    break;

  case SEQ_END:
    WBM_SEQ_End( p_seqwk );
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
 *	@brief  WIFI���  �o�^��������
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSubSeq_UnRegister( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  {
    SEQ_CHECK_CUP_STATUS,
    SEQ_START_NOT_CUP_MSG,
    SEQ_START_UNREGISTER_MSG,
    SEQ_START_UNREGISTER_LIST,
    SEQ_SELECT_UNREGISTER_LIST,
    SEQ_START_CONFIRM_MSG,
    SEQ_START_CONFIRM_LIST,
    SEQ_SELECT_CONFIRM_LIST,

    //�Z�[�u
    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,

    SEQ_START_UNLOCK_MSG,
    //GPF���M
    SEQ_START_SEND_MSG, 
    SEQ_SEND_GPF_CUPSTATUS,
    SEQ_WAIT_GPF_CUPSTATUS,

    SEQ_END,
    SEQ_WAIT_MSG,

  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECK_CUP_STATUS:
    if( p_wk->p_param->p_gpf_data->WifiMatchUpState == DREAM_WORLD_MATCHUP_ENTRY ||
        p_wk->p_param->p_gpf_data->WifiMatchUpState == DREAM_WORLD_MATCHUP_SIGNUP )
    { 
      p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_UNREGISTER_RET_FALSE;
      *p_seq  = SEQ_START_UNREGISTER_MSG;
    }
    else
    { 
      p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_UNREGISTER_RET_NONE;
      *p_seq  = SEQ_START_NOT_CUP_MSG;
    }
    break;
  case SEQ_START_NOT_CUP_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_27, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_END );
    break;
  case SEQ_START_UNREGISTER_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_28, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_UNREGISTER_LIST );
    break;
  case SEQ_START_UNREGISTER_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO_DEFAULT_NO );
    *p_seq     = SEQ_SELECT_UNREGISTER_LIST;
    break;
  case SEQ_SELECT_UNREGISTER_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_START_CONFIRM_MSG;
          break;
        case 1: //������
          *p_seq  = SEQ_END;
          break;
        }
      }
    }
    break;
  case SEQ_START_CONFIRM_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_29, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_CONFIRM_LIST );
    break;
  case SEQ_START_CONFIRM_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO_DEFAULT_NO );
    *p_seq     = SEQ_SELECT_CONFIRM_LIST;
    break;
  case SEQ_SELECT_CONFIRM_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //�͂�
          *p_seq  = SEQ_START_SAVE_MSG;
          break;
        case 1: //������
          *p_seq  = SEQ_END;
          break;
        }
      }
    }
    break;


  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    break;

  case SEQ_START_SAVE:
    {
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      BATTLE_BOX_SAVE_OffLockFlg( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI );

      //���^�C����Ԃɂ��ăZ�[�u
      Regulation_SetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_RETIRE );

      GAMEDATA_SaveAsyncStart( p_param->p_param->p_game_data );
      *p_seq  = SEQ_WAIT_SAVE;
    }
    break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT ret = GAMEDATA_SaveAsyncMain( p_param->p_param->p_game_data );
      if( ret == SAVE_RESULT_OK )
      { 
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_UNREGISTER_RET_TRUE;
        *p_seq  = SEQ_START_UNLOCK_MSG;
      }
      else if( ret == SAVE_RESULT_NG )
      { 
        *p_seq  = SEQ_END;
      }
    }
    break;

  case SEQ_START_UNLOCK_MSG:
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_26, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SEND_MSG );
    break;


  case SEQ_START_SEND_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_18, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_SEND_GPF_CUPSTATUS );
    break;

  case SEQ_SEND_GPF_CUPSTATUS:
    { 
      DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA data;
      GFL_STD_MemClear( &data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      data.WifiMatchUpState = DREAM_WORLD_MATCHUP_RETIRE;
      p_wk->p_param->p_gpf_data->WifiMatchUpState = DREAM_WORLD_MATCHUP_RETIRE;
      WIFIBATTLEMATCH_NET_StartSendGpfData( p_wk->p_net, &data, HEAPID_WIFIBATTLEMATCH_CORE ); 
      *p_seq  = SEQ_WAIT_GPF_CUPSTATUS;
    }
    break;

  case SEQ_WAIT_GPF_CUPSTATUS:
    { 
      WIFIBATTLEMATCH_SEND_GPFDATA_RET ret;
      
      ret = WIFIBATTLEMATCH_NET_WaitSendGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS )
      { 
        *p_seq  = SEQ_END;
      }
      else if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY )
      {
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_UNREGISTER_RET_SERVER;
        *p_seq  = SEQ_END;
      }
      //�����̓T�u�V�[�P���X�Ȃ̂ŃG���[�����͂��̏�ōs��
    } 
    break;


  case SEQ_END:
    WBM_SEQ_End( p_seqwk );
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
 *	@brief  �s���`�F�b�N�T�[�o�[��1�C�����菐����Ⴄ
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSubSeq_EvilCheck( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START,
    SEQ_START_EVIL_CHECK,
    SEQ_WAIT_EVIL_CHECK,
    SEQ_END_CHECK,
    SEQ_END,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	      = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param    = p_wk->p_param;
  WIFIBATTLEMATCH_ENEMYDATA   *p_my_data  = p_param->p_player_data;

  switch( *p_seq )
  { 
  case SEQ_START:
    { 
      OS_TFPrintf( 3, "�����̃|�P���������擾�J�n\n" );
    }
    (*p_seq)  = SEQ_START_EVIL_CHECK;
    break;

  case SEQ_START_EVIL_CHECK:
    { 
      POKEPARTY *p_my_party = (POKEPARTY*)p_param->p_player_data->pokeparty;
      GFL_STD_MemClear( &p_wk->evilecheck_data, sizeof(WIFIBATTLEMATCH_NET_EVILCHECK_DATA));
      WIFIBATTLEMATCH_NET_StartEvilCheck( p_wk->p_net, p_my_party, WIFIBATTLEMATCH_NET_EVILCHECK_TYPE_PARTY );
      (*p_seq)  = SEQ_WAIT_EVIL_CHECK;
      DEBUG_WIFICUP_Printf( "EvilCheck Start\n" );
    }
    break;

  case SEQ_WAIT_EVIL_CHECK:
    {
      WIFIBATTLEMATCH_NET_EVILCHECK_RET ret;
      ret = WIFIBATTLEMATCH_NET_WaitEvilCheck( p_wk->p_net, &p_wk->evilecheck_data );
      if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_SUCCESS )
      { 
        DEBUG_WIFICUP_Printf( "EvilCheck Success\n" );
        (*p_seq)  = SEQ_END_CHECK;
      }
      else if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_ERROR )
      { 
        DEBUG_WIFICUP_Printf( "EvilCheck Error\n" );
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_EVILCHECK_RET_NET_ERR;
        *p_seq  = SEQ_END;
      }
    }
    break;

  case SEQ_END_CHECK:
    //�f�[�^���i�[
    OS_TFPrintf( 3, "�|�P��������[�s���ԍ�%d]status=%d\n", p_wk->evilecheck_data.poke_result[0], p_wk->evilecheck_data.status_code );
    if( p_wk->evilecheck_data.status_code == 0 )
    { 
      OS_TFPrintf( 3, "-=-=-=�|�P�����ؖ�-=-=-=\n" );
      {
        int len = 0;
        for(len = 0; len < 128; len++ )
        {
          OS_TFPrintf( 3,"%x ", (u8) p_wk->evilecheck_data.sign[len] );
          if( (len + 1) % 16 == 0 ) OS_TFPrintf( 3,"\n");
        }
        OS_TFPrintf( 3, "\n" );
      }
      OS_TFPrintf( 3, "-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");

      //������ۑ�
      Util_SetMyDataSign( p_my_data, &p_wk->evilecheck_data );

      DEBUG_WIFICUP_Printf( "EvilCheck �����I�I\n" );
      p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_EVILCHECK_RET_SUCCESS;
      *p_seq  = SEQ_END;
    }
    else
    { 
      DEBUG_WIFICUP_Printf( "EvilCheck �s���I�I\n");
      p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_EVILCHECK_RET_DARTY;
      *p_seq  = SEQ_END;
    }
    break;

  case SEQ_END:
    WBM_SEQ_End( p_seqwk );
    break;
  }


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
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  if( p_wk->p_playerinfo == NULL )
  {
    MYSTATUS    *p_my;
    GFL_CLUNIT	*p_unit;
    SAVE_CONTROL_WORK *p_sv;
    REGULATION_SAVEDATA *p_reg_sv;
    REGULATION_VIEWDATA *p_reg_view;

    PLAYERINFO_WIFICUP_DATA info_setup;

    const REGULATION_CARDDATA *cp_reg_card  = p_wk->p_reg;

    p_my  = GAMEDATA_GetMyStatus( p_wk->p_param->p_param->p_game_data); 
    p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    p_sv = GAMEDATA_GetSaveControlWork( p_wk->p_param->p_param->p_game_data );
    p_reg_sv  = SaveData_GetRegulationSaveData( p_sv );
    p_reg_view  = RegulationSaveData_GetRegulationView( p_reg_sv, REGULATION_CARD_TYPE_WIFI );

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
    info_setup.rate = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_RATE );
    info_setup.btl_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_WIN ) + RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_LOSE );
    info_setup.bgm_no = Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_BGM );


    if( info_setup.btl_cnt == 0 &&  info_setup.rate == 0 )
    { 
      info_setup.rate = WIFIBATTLEMATCH_GDB_DEFAULT_RATEING;
    }

    p_wk->p_playerinfo	= PLAYERINFO_WIFI_Init( &info_setup, FALSE, p_my, p_unit, p_wk->p_res, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, p_reg_view, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̏���j��
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
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
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk  ���[�N
 *
 *	@return TRUE�Ŋ���  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL Util_PlayerInfo_MoveIn( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  return PLAYERINFO_MoveMain( p_wk->p_playerinfo );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h���X���C�h�A�E�g
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk   ���[�N
 *
 *	@return TRUE�Ŋ���  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL Util_PlayerInfo_MoveOut( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  return PLAYERINFO_MoveOutMain( p_wk->p_playerinfo );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h�쐬  ���łɃX���C�h�C�����Ă����
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_CreateStay( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  Util_PlayerInfo_Create( p_wk );

  while( !Util_PlayerInfo_MoveIn( p_wk ) )
  { 

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h�̃f�[�^���X�V
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk
 *	@param	type 
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_RenewalData( WIFIBATTLEMATCH_WIFI_WORK *p_wk, PLAYERINFO_WIFI_UPDATE_TYPE type )
{ 
  if( p_wk->p_playerinfo )
  {
    PLAYERINFO_WIFI_RenewalData( p_wk->p_playerinfo, type, p_wk->p_msg, p_wk->p_que, p_wk->p_font, HEAPID_WIFIBATTLEMATCH_CORE );
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�҂̏���\��
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_MatchInfo_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_enemy_data )
{ 
  if( p_wk->p_matchinfo == NULL )
  { 
    GFL_CLUNIT  *p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    REGULATION_CARD_BGM_TYPE type = Regulation_GetCardParam( p_wk->p_reg, REGULATION_CARD_BGM );
    p_wk->p_matchinfo		= MATCHINFO_Init( cp_enemy_data, p_unit, p_wk->p_res, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, WIFIBATTLEMATCH_TYPE_WIFICUP, TRUE, type, HEAPID_WIFIBATTLEMATCH_CORE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�҂̏���j��
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_MatchInfo_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
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
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk  ���[�N
 *
 *	@return TRUE�Ŋ���  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL Util_MatchInfo_Main( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{
  return MATCHINFO_MoveMain( p_wk->p_matchinfo );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�҃J�[�h������
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_Matchinfo_Clear( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{
  if( p_wk->p_matchinfo )
  {
    Util_MatchInfo_Delete( p_wk );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X��\��
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_BtlBox_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  if( p_wk->p_btlbox == NULL )
  { 
    GFL_CLUNIT  *p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->p_param->p_param->p_game_data );
    BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
    p_wk->p_btlbox  = WBM_BTLBOX_Init( p_bbox_save, p_unit, p_wk->p_res, p_wk->p_small_font, p_wk->p_que, p_wk->p_msg, HEAPID_WIFIBATTLEMATCH_CORE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X��\��
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_BtlBox_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  if( p_wk->p_btlbox )
  { 
    WBM_BTLBOX_Exit( p_wk->p_btlbox );
    p_wk->p_btlbox  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X���ړ�
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 *	@retval TRUE�ړ����� FALSE�ړ���
 */
//-----------------------------------------------------------------------------
static BOOL Util_BtlBox_MoveIn( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  return WBM_BTLBOX_MoveInMain( p_wk->p_btlbox );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���{�b�N�X���ړ�
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 *	@retval TRUE�ړ����� FALSE�ړ���
 */
//-----------------------------------------------------------------------------
static BOOL Util_BtlBox_MoveOut( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  return WBM_BTLBOX_MoveOutMain( p_wk->p_btlbox );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g������
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk  ���[�N
 *	@param	type                            ���X�g�̎��
 */
//-----------------------------------------------------------------------------
static void Util_List_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk, UTIL_LIST_TYPE type )
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
    case UTIL_LIST_TYPE_YESNO_DEFAULT_NO:
      setup.default_idx = 1;
      /* fallthrough */
    case UTIL_LIST_TYPE_YESNO:
      setup.strID[0]= WIFIMATCH_WIFI_SELECT_04;
      setup.strID[1]= WIFIMATCH_WIFI_SELECT_05;
      setup.list_max= 2;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 1;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      break;

    case UTIL_LIST_TYPE_JOIN:
      setup.strID[0]= WIFIMATCH_WIFI_SELECT_01;
      setup.strID[1]= WIFIMATCH_WIFI_SELECT_02;
      setup.strID[2]= WIFIMATCH_WIFI_SELECT_03;
      setup.list_max= 3;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 2;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      break;

    case UTIL_LIST_TYPE_DECIDE:
      setup.strID[0]= WIFIMATCH_WIFI_SELECT_06;
      setup.strID[1]= WIFIMATCH_WIFI_SELECT_07;
      setup.list_max= 2;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 1;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      break;

    case UTIL_LIST_TYPE_CUPMENU:
      setup.strID[0]= WIFIMATCH_WIFI_SELECT_08;
      setup.strID[1]= WIFIMATCH_WIFI_SELECT_09;
      setup.strID[2]= WIFIMATCH_WIFI_SELECT_10;
      setup.list_max= 3;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 2;
      p_wk->p_list  = WBM_LIST_InitEx( &setup, 32/2 - 26/2, (24-6)/2 - 3*2/2, 26, 3*2, HEAPID_WIFIBATTLEMATCH_CORE );
      break;

    case UTIL_LIST_TYPE_CUPMENU_CONT:
      setup.strID[0]= WIFIMATCH_WIFI_SELECT_11;
      setup.strID[1]= WIFIMATCH_WIFI_SELECT_12;
      setup.strID[2]= WIFIMATCH_WIFI_SELECT_13;
      setup.list_max= 3;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 2;
      p_wk->p_list  = WBM_LIST_InitEx( &setup, 32/2 - 26/2, (24-6)/2 - 3*2/2, 26, 3*2, HEAPID_WIFIBATTLEMATCH_CORE );
      break;
    }

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�j��
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_List_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
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
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 *
 *	@return �I����������
 */
//-----------------------------------------------------------------------------
static u32 Util_List_Main( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  if( p_wk->p_list )
  { 
    return WBM_LIST_Main( p_wk->p_list );
  }
  else
  { 
    DEBUG_WIFICUP_Printf( "List not exist !!!\n" );
    return WBM_LIST_SELECT_NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �T�u�V�[�P���X�X�^�[�g
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk   ���[�N
 *	@param	seq_function                      �T�u�V�[�P���X
 */
//-----------------------------------------------------------------------------
static void Util_SubSeq_Start( WIFIBATTLEMATCH_WIFI_WORK *p_wk, WBM_SEQ_FUNCTION seq_function )
{ 
  WBM_SEQ_SetNext( p_wk->p_subseq, seq_function );
  p_wk->subseq_ret = WBM_WIFI_SUBSEQ_RET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �T�u�V�[�P���X���C��
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 *
 *	@return WBM_WIFI_SUBSEQ_RET_NONE���쒆  ����ȊO�̂Ƃ��̓T�u�V�[�P���X���I�����߂�l��Ԃ��Ă���
 */
//-----------------------------------------------------------------------------
static WBM_WIFI_SUBSEQ_RET Util_SubSeq_Main( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  WBM_SEQ_Main( p_wk->p_subseq );
  if( WBM_SEQ_IsEnd( p_wk->p_subseq ) )
  {
    return p_wk->subseq_ret;
  }
  else
  { 
    return WBM_WIFI_SUBSEQ_RET_NONE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̃f�[�^��������
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_InitMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  GFL_STD_MemClear( p_my_data, sizeof(WIFIBATTLEMATCH_ENEMYDATA) );

  { 
    SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->p_param->p_param->p_game_data );
    BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
    POKEPARTY         *p_party  =  BATTLE_BOX_SAVE_MakePokeParty( p_bbox_save, HEAPID_WIFIBATTLEMATCH_CORE );

    PokeParty_InitWork( (POKEPARTY*)p_my_data->pokeparty );

    GFL_STD_MemCopy( p_party, (POKEPARTY*)p_my_data->pokeparty, PokeParty_GetWorkSize() );

    GFL_HEAP_FreeMemory( p_party );
  }

  p_my_data->btl_server_version  = BTL_NET_SERVER_VERSION;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃f�[�^���i�[
 *	        SAKE GPF����f�[�^��������Ă��Ă��邱��
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data  �f�[�^�i�[��
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *cp_wk      ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_SetMyDataInfo( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, const WIFIBATTLEMATCH_WIFI_WORK *cp_wk )
{ 
  SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork(cp_wk->p_param->p_param->p_game_data);
  {
    MYSTATUS  *p_my;
    p_my  = GAMEDATA_GetMyStatus(cp_wk->p_param->p_param->p_game_data);
    MyStatus_Copy( p_my, (MYSTATUS*)p_my_data->mystatus );
  }
  { 
    const MYPMS_DATA *cp_mypms;
    cp_mypms        = SaveData_GetMyPmsDataConst( p_sv );
    MYPMS_GetPms( cp_mypms, MYPMS_PMS_TYPE_INTRODUCTION, &p_my_data->pms );
  }
  p_my_data->win_cnt    = cp_wk->p_param->p_wifi_sake_data->win;
  p_my_data->lose_cnt   = cp_wk->p_param->p_wifi_sake_data->lose;
  p_my_data->rate       = cp_wk->p_param->p_wifi_sake_data->rate;

  p_my_data->sake_recordID = WIFIBATTLEMATCH_GDB_GetRecordID( cp_wk->p_net );

  { 
    //PID
    WIFI_LIST *p_wlist = GAMEDATA_GetWiFiList( cp_wk->p_param->p_param->p_game_data );
    p_my_data->profileID  = WifiList_GetMyGSID( p_wlist );
  }

  { 
    const REGULATION_CARDDATA *cp_reg_card  = cp_wk->p_reg;
    p_my_data->wificup_no      = Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_CUPNO );
    p_my_data->reg_crc  = Regulation_GetCrc( cp_reg_card );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ������ǉ�
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data      ���[�N
 *	@param	WIFIBATTLEMATCH_NET_EVILCHECK_DATA  *cp_evilecheck_data �s���`�F�b�N
 *	@param	index                                                   ���Ԗڂ̃|�P������
 *
 */
//-----------------------------------------------------------------------------
static void Util_SetMyDataSign( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, const WIFIBATTLEMATCH_NET_EVILCHECK_DATA  *cp_evilecheck_data )
{ 
  OS_TFPrintf( 3, "������ǉ�\n" );
  GFL_STD_MemCopy( cp_evilecheck_data->sign, p_my_data->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����ؖ�
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data  �ؖ�����������̃f�[�^
 *	@param	heapID  �q�[�vID
 *
 *	@return TRUE�ŏؖ�  FALSE�ŕs��
 */
//-----------------------------------------------------------------------------
static BOOL Util_VerifyPokeData( WIFIBATTLEMATCH_ENEMYDATA *p_data, POKEPARTY *p_party, HEAPID heapID )
{ 
  int i;
  void *p_buff;
  POKEMON_PARAM *p_pp;
  BOOL ret  = TRUE;

  DEBUG_WIFICUP_Printf( "�|�P���������J�n�@�S����[%d]�C\n", PokeParty_GetPokeCount( p_party ) );
  OS_TFPrintf( 3, "����̃|�P���������`�F�b�N�J�n\n" );

  p_buff  = NHTTP_RAP_EVILCHECK_CreateVerifyPokeBuffer( POKETOOL_GetWorkSize(), PokeParty_GetPokeCount(p_party), GFL_HEAP_LOWID(heapID) );

  for( i = 0; i < PokeParty_GetPokeCount( p_party ); i++ )
  { 
    p_pp  = PokeParty_GetMemberPointer( p_party, i);

    NHTTP_RAP_EVILCHECK_AddPokeVerifyPokeBuffer( p_buff, p_pp, POKETOOL_GetWorkSize(), i );
  }

  ret = NHTTP_RAP_EVILCHECK_VerifySign( p_buff, POKETOOL_GetWorkSize(), PokeParty_GetPokeCount(p_party), p_data->sign, GFL_HEAP_LOWID(heapID) );

  NHTTP_RAP_EVILCHECK_DeleteVerifyPokeBuffer( p_buff );

  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����p�[�e�B�̈�v
 *
 *	@param	const POKEPARTY *cp_pokeparty_1 ��rA
 *	@param	POKEPARTY *cp_pokeparty_2       ��rB
 *
 *	@return TRUE��v  FALSE�s��v
 */
//-----------------------------------------------------------------------------
static BOOL Util_ComarepPokeParty( const POKEPARTY *cp_pokeparty_1, const POKEPARTY *cp_pokeparty_2 )
{ 
  const u32 max1  = PokeParty_GetPokeCount(cp_pokeparty_1);
  const u32 max2  = PokeParty_GetPokeCount(cp_pokeparty_2 );
  if( max1 == max2 )
  { 
    int i;
    POKEMON_PARAM *p_pp1;
    POKEMON_PARAM *p_pp2;
    POKEMON_PASO_PARAM  *p_ppp1;
    POKEMON_PASO_PARAM  *p_ppp2;
    for( i = 0; i < max1; i++ )
    { 
      p_pp1 = PokeParty_GetMemberPointer( cp_pokeparty_1, i );
      p_pp2 = PokeParty_GetMemberPointer( cp_pokeparty_2, i );

      p_ppp1  = PP_GetPPPPointer( p_pp1 );
      p_ppp2  = PP_GetPPPPointer( p_pp2 );

      if( GFL_STD_MemComp( p_ppp1, p_ppp2, POKETOOL_GetPPPWorkSize() ) != 0 )
      {
        DEBUG_WIFICUP_Printf( "Cnt%d Failed !!!\n", i );
        return FALSE;
      }
    }
    return TRUE;
  }

  DEBUG_WIFICUP_Printf( "%d != %d !!!\n", max1, max2 );
  
  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �L�����Z������
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N
 *	@param	is_cancel_enable TRUE�Ȃ�΃L�����Z���������Ȃ��Ă悢 FALSE�Ȃ�΃_��
 *	@param  can_disconnect  �ؒf���Ă悢�̂Ȃ��TRUE
 *
 *	@return �L�����Z�����
 */
//-----------------------------------------------------------------------------
static UTIL_CANCEL_STATE Util_Cancel_Seq( WIFIBATTLEMATCH_WIFI_WORK *p_wk, BOOL is_cancel_enable, BOOL can_disconnect )
{
  enum
  {
    SEQ_CHECK_CANCEL,
    SEQ_START_CANCEL_MSG,
    SEQ_WAIT_CANCEL_MSG,
    SEQ_START_LIST,
    SEQ_WAIT_LIST,

    SEQ_START_DISCONNECT_MSG,
    SEQ_SEND_CANCEL,
    SEQ_END,
  };

  switch( p_wk->cancel_seq )
  {
  case SEQ_CHECK_CANCEL:
    if( is_cancel_enable && (GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL) )
    {
      p_wk->cancel_seq++;
    }
    break;
  case SEQ_START_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_011, WBM_TEXT_TYPE_STREAM );
    p_wk->cancel_seq++;
    break;
  case SEQ_WAIT_CANCEL_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    {
      p_wk->cancel_seq++;
    }
    break;
  case SEQ_START_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    p_wk->cancel_seq++;
    break;
  case SEQ_WAIT_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0:
          p_wk->cancel_seq  = SEQ_START_DISCONNECT_MSG;
          break;
        case 1:
          WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_009, WBM_TEXT_TYPE_QUE );
          p_wk->cancel_seq  = 0;
          return UTIL_CANCEL_STATE_EXIT;
        }
      }
    }
    break;
  case SEQ_START_DISCONNECT_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_020, WBM_TEXT_TYPE_WAIT );
    p_wk->cancel_seq++;
    break;
  case SEQ_SEND_CANCEL:
    if( can_disconnect )
    {
      if( WIFIBATTLEMATCH_NET_SendMatchCancel( p_wk->p_net ) )
      {
        p_wk->cancel_seq++;
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
      }
    }
    break;
  case SEQ_END:
    p_wk->cancel_seq  = 0;
    return UTIL_CANCEL_STATE_DECIDE;
  }

  if( p_wk->cancel_seq == 0 )
  {
    return UTIL_CANCEL_STATE_NONE;
  }
  else
  {
    return UTIL_CANCEL_STATE_WAIT;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�R�A�Z�[�u
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ���[�N 
 *
 *	@return TRUE�ŕύX����  FALSE�ŕύX�Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Util_SaveScore( WIFIBATTLEMATCH_WIFI_WORK *p_wk, const WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA *cp_data )
{
  BOOL ret;

  if( RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_RATE ) == cp_data->rate
  && RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_WIN ) == cp_data->win
  && RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_LOSE ) == cp_data->lose )
  {
    ret = FALSE;
  }
  else
  {
    ret = TRUE;
  }


  RNDMATCH_SetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_RATE, cp_data->rate );
  RNDMATCH_SetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_WIN, cp_data->win );
  RNDMATCH_SetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_LOSE, cp_data->lose );


  return ret;
}

//=============================================================================
/**
 *  �f�o�b�O
 */
//=============================================================================

#ifdef DEBUGWIN_USE

static void DEBUGWIN_Init( WIFIBATTLEMATCH_WIFI_WORK *p_wk, HEAPID heapID )
{ 
  DEBUGWIN_InitProc( GFL_BG_FRAME0_M, p_wk->p_font );
  DEBUGWIN_ChangeLetterColor( 0,31,0 );

  DEBUGWIN_REG_Init( p_wk->p_reg, heapID );
  DEBUGWIN_WIFISCORE_Init( heapID );
  DEBUGWIN_SAKERECORD_Init( p_wk->p_param->p_record_data, heapID );
  DEBUGWIN_REPORT_Init( heapID );
  DEBUGWIN_BTLBOX_Init( heapID );
  DEBUGWIN_BTLBGM_Init( heapID );
  DEBUGWIN_ATLAS_Init( heapID );
  DEBUGWIN_ETC_InitEx( heapID, (POKEPARTY*)p_wk->p_param->p_player_data->pokeparty );
}
static void DEBUGWIN_Exit( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{
  DEBUGWIN_ETC_Exit();
  DEBUGWIN_ATLAS_Exit();
  DEBUGWIN_BTLBGM_Exit();
  DEBUGWIN_BTLBOX_Exit();
  DEBUGWIN_REPORT_Exit();
  DEBUGWIN_SAKERECORD_Exit();
  DEBUGWIN_WIFISCORE_Exit();
  DEBUGWIN_REG_Exit();

  DEBUGWIN_ExitProc();
}
#endif
