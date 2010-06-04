//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_core.c
 *	@brief	WIFI�̃o�g���}�b�`�R�A���  �����_���}�b�`�V�[�P���X
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
#include "pokeicon/pokeicon.h"
#include "gamesystem/game_data.h"
#include "system/net_err.h"
#include "battle/btl_net.h" //BTL_NET_SERVER_VERSION
#include "net/dwc_rapcommon.h"

//	�A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "msg/msg_wifi_match.h"

//	�����\��
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"

//  �Z�[�u�f�[�^
#include "savedata/dreamworld_data.h"
#include "savedata/my_pms_data.h"
#include "savedata/etc_save.h"

//WIFI�o�g���}�b�`�̃��W���[��
#include "wifibattlematch_graphic.h"
#include "wifibattlematch_view.h"
#include "wifibattlematch_net.h"
#include "wifibattlematch_util.h"
#include "wifibattlematch_snd.h"

//�O�����J
#include "wifibattlematch_core.h"

#include "debug/debug_flg.h"
#include "debug/debug_nagihashi.h"

//-------------------------------------
///	�I�[�o�[���C
//=====================================
FS_EXTERN_OVERLAY(ui_common);
FS_EXTERN_OVERLAY(dpw_common);

//-------------------------------------
///	�f�o�b�O�}�N��
//=====================================
#ifdef PM_DEBUG
#define DEBUGWIN_USE

//#if defined(DEBUG_ONLY_FOR_toru_nagihashi)||defined(DEBUG_ONLY_FOR_shimoyamada)
//#define DEBUG_GPF_PASS
//#endif

//#define DEBUG_DIRTYCHECK_PASS



#define DEBUG_FLAG_GPF_PASS_ON    //GPF���X���[����t���O�̎g�p

#endif //PM_DEBUG




//�f�o�b�OWIN�C���N���[�h
#ifdef DEBUGWIN_USE
//�f�o�b�O
#include "debug/debugwin_sys.h"
#include "wifibattlematch_debug.h"


#define DEBUGWIN_GROUP_CARD (20)
#define DEBUGWIN_GROUP_SCORE (30)

#endif//DEBUGWIN_USE

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�V���N
//=====================================
#define ENEMYDATA_WAIT_SYNC    (60*5)
#define ENEMYDATA_WAIT_SYNC_RATE    (0)
#define MATCHING_MSG_WAIT_SYNC (120)
#define SELECTPOKE_MSG_WAIT_SYNC (60)

#define MATCHING_TIMEOUT_SYNC   (60*20)

//-------------------------------------
///	�q�[�v�T�C�Y
//=====================================
#define WBM_RND_RATE_HEAP_SIZE  (0x30000)

//-------------------------------------
///	�T�u�V�[�P���X�߂�l
//=====================================
typedef enum
{
  WBM_RND_SUBSEQ_EVILCHECK_RET_SUCCESS  = 0,  //����
  WBM_RND_SUBSEQ_EVILCHECK_RET_DARTY,         //�P�̂ł��s���|�P����������
  WBM_RND_SUBSEQ_EVILCHECK_RET_NET_ERR,       //�l�b�g�G���[

  WBM_RND_SUBSEQ_RET_NONE  = 0xFFFF,          //�Ȃ�
} WBM_RND_SUBSEQ_RET;


//=============================================================================
/**
 *        ��`
*/
//=============================================================================
//-------------------------------------
///	���C�����[�N�O���錾
//=====================================
typedef struct
{
	//�O���t�B�b�N�ݒ�
	WIFIBATTLEMATCH_GRAPHIC_WORK	*p_graphic;

  //���\�[�X
  WIFIBATTLEMATCH_VIEW_RESOURCE *p_res;

	//���ʂŎg���t�H���g
	GFL_FONT			                *p_font;

	//���ʂŎg���L���[
	PRINT_QUE				              *p_que;

	//���ʂŎg�����b�Z�[�W
	GFL_MSGDATA	          	    	*p_msg;

	//���ʂŎg���P��
	WORDSET				              	*p_word;

	//���ʏ��
	PLAYERINFO_WORK             	*p_playerinfo;

	//�ΐ�ҏ��
	MATCHINFO_WORK              	*p_matchinfo;

  //�ҋ@�A�C�R��
  WBM_WAITICON_WORK             *p_wait;

  //�e�L�X�g��
  WBM_TEXT_WORK                 *p_text;

  //���X�g
  WBM_LIST_WORK                 *p_list;
  u32                           list_type;

  //�l�b�g
  WIFIBATTLEMATCH_NET_WORK      *p_net;

  //���C���V�[�P���X
  WBM_SEQ_WORK                  *p_seq;

  //�T�u�V�[�P���X
  WBM_SEQ_WORK                  *p_subseq;
  WBM_RND_SUBSEQ_RET            subseq_ret;

  //����
  WIFIBATTLEMATCH_CORE_PARAM    *p_param;

  //�s���`�F�b�N�̂Ƃ��Ɏg�p����o�b�t�@�i����ȊO�͎Q�Ƃ��Ȃ��j
  WIFIBATTLEMATCH_NET_EVILCHECK_DATA  evilecheck_data;

  //�E�G�C�g
  u32 cnt;

  //�L�����Z���V�[�P���X
  u32 cancel_seq;
  u32 match_timeout;

#ifdef DEBUGWIN_USE
  u32 playerinfo_mode;
  u32 matchinfo_mode;
  u32 playerinfo_rank;
  u32 matchinfo_rank;
  u32 waiticon_draw;
#endif //DEBUGWIN_USE

} WIFIBATTLEMATCH_RND_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT WIFIBATTLEMATCH_RND_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_RND_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_RND_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	�����_���}�b�`�V�[�P���X�֐�
//=====================================
static void WbmRndSeq_Init( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//���[�e�B���O����
static void WbmRndSeq_Rate_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_StartMatching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_EndRec( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_CupContinue( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_DisConnextSendTime( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//�t���[����
static void WbmRndSeq_Free_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Free_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Free_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Free_EndRec( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Free_CupContinue( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Free_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//�G���[����
static void WbmRndSeq_Err_ReturnLogin( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//�ؒf
static void WbmRndSeq_DisConnectEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	�T�u�V�[�P���X�֐�
//   Util_SubSeq_Start�����g���A�V�[�P���X��œ�������
//=====================================
static void WbmRndSubSeq_EvilCheck( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	�֗�
//=====================================
//�����̃J�[�h�쐬
static void Util_PlayerInfo_Create( WIFIBATTLEMATCH_RND_WORK *p_wk, WIFIBATTLEMATCH_CORE_RETMODE mode );
static void Util_PlayerInfo_Delete( WIFIBATTLEMATCH_RND_WORK *p_wk );
static BOOL Util_PlayerInfo_Move( WIFIBATTLEMATCH_RND_WORK *p_wk );
static void Util_PlayerInfo_CreateStay( WIFIBATTLEMATCH_RND_WORK *p_wk, WIFIBATTLEMATCH_CORE_RETMODE mode );
//�ΐ푊��̃J�[�h�쐬
static void Util_MatchInfo_Create( WIFIBATTLEMATCH_RND_WORK *p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_enemy_data );
static void Util_MatchInfo_Delete( WIFIBATTLEMATCH_RND_WORK *p_wk );
static BOOL Util_MatchInfo_Main( WIFIBATTLEMATCH_RND_WORK *p_wk );
//�X�R�A�Z�[�u
static void Util_SaveRateScore( RNDMATCH_DATA *p_save, WIFIBATTLEMATCH_BTLRULE btl_rule, const WIFIBATTLEMATCH_RND_WORK *cp_wk );
static void Util_SaveFreeScore( RNDMATCH_DATA *p_rndmatch, WIFIBATTLEMATCH_BTLRULE btl_rule, BtlResult btl_result );
//�I�����쐬
typedef enum
{ 
  UTIL_LIST_TYPE_YESNO,
  UTIL_LIST_TYPE_FREERATE,
}UTIL_LIST_TYPE;
static void Util_List_Create( WIFIBATTLEMATCH_RND_WORK *p_wk, UTIL_LIST_TYPE type );
static void Util_List_Delete( WIFIBATTLEMATCH_RND_WORK *p_wk );
static u32 Util_List_Main( WIFIBATTLEMATCH_RND_WORK *p_wk );
//�}�b�`���O�p�f�[�^�쐬
static void Util_Matchkey_SetData( WIFIBATTLEMATCH_MATCH_KEY_DATA *p_data, const WIFIBATTLEMATCH_RND_WORK *cp_wk );
//�����̃f�[�^�쐬
static void Util_InitMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, WIFIBATTLEMATCH_RND_WORK *p_wk );
static void Util_RenewalMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, WIFIBATTLEMATCH_RND_WORK *p_wk );
//�T�u�V�[�P���X
static void Util_SubSeq_Start( WIFIBATTLEMATCH_RND_WORK *p_wk, WBM_SEQ_FUNCTION seq_function );
static WBM_RND_SUBSEQ_RET Util_SubSeq_Main( WIFIBATTLEMATCH_RND_WORK *p_wk );
//�|�P�����ؖ�
static BOOL Util_VerifyPokeData( WIFIBATTLEMATCH_ENEMYDATA *p_data, HEAPID heapID );
//�L�����Z���V�[�P���X
typedef enum
{
  UTIL_CANCEL_STATE_NONE, //�������Ă��Ȃ�
  UTIL_CANCEL_STATE_WAIT, //�L�����Z���ҋ@���i�͂��A�������҂����j
  UTIL_CANCEL_STATE_EXIT, //�ҋ@�����𔲂���
  UTIL_CANCEL_STATE_DECIDE, //�L�����Z�����m��
}UTIL_CANCEL_STATE;
static UTIL_CANCEL_STATE Util_Cancel_Seq( WIFIBATTLEMATCH_RND_WORK *p_wk, BOOL is_cancel );


//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef DEBUGWIN_USE
static void DEBUGWIN_CARD_Init( WIFIBATTLEMATCH_RND_WORK *p_wk );
static void DEBUGWIN_CARD_Exit( WIFIBATTLEMATCH_RND_WORK *p_wk );
#endif //DEBUGWIN_USE
//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	WifiBattleMatchRnd_ProcData =
{	
	WIFIBATTLEMATCH_RND_PROC_Init,
	WIFIBATTLEMATCH_RND_PROC_Main,
	WIFIBATTLEMATCH_RND_PROC_Exit,
};

//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`�����_���ΐ�	�R�A�v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_RND_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	WIFIBATTLEMATCH_RND_WORK	*p_wk;
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;


	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common));

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_CORE, 0x35000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFIBATTLEMATCH_RND_WORK), HEAPID_WIFIBATTLEMATCH_CORE );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_RND_WORK) );	
  p_wk->p_param = p_param;

	//�O���t�B�b�N�ݒ�
	p_wk->p_graphic	= WIFIBATTLEMATCH_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_WIFIBATTLEMATCH_CORE );

  //���\�[�X�ǂݍ���
  p_wk->p_res     = WIFIBATTLEMATCH_VIEW_LoadResource( WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic ), WIFIBATTLEMATCH_VIEW_RES_MODE_RANDOM, HEAPID_WIFIBATTLEMATCH_CORE );

  //�����̃f�[�^��ݒ�
  Util_InitMyData( p_param->p_player_data, p_wk );

	//���ʃ��W���[���̍쐬
	p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
												NARC_message_wifi_match_dat, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_word	= WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_COUNTRY_BUFLEN, HEAPID_WIFIBATTLEMATCH_CORE );

	//���W���[���̍쐬
  p_wk->p_net   = WIFIBATTLEMATCH_NET_Init( p_wk->p_param->p_net_data,
      p_param->p_param->p_game_data, p_param->p_svl_result, HEAPID_WIFIBATTLEMATCH_CORE );
  p_wk->p_text  = WBM_TEXT_Init( BG_FRAME_M_TEXT, PLT_FONT_M, PLT_TEXT_M, CGR_OFS_M_TEXT, p_wk->p_que, p_wk->p_font, HEAPID_WIFIBATTLEMATCH_CORE );
  p_wk->p_seq   = WBM_SEQ_Init( p_wk, WbmRndSeq_Init, HEAPID_WIFIBATTLEMATCH_CORE );
  p_wk->p_subseq   = WBM_SEQ_Init( p_wk, NULL, HEAPID_WIFIBATTLEMATCH_CORE );

  { 
    GFL_CLUNIT  *p_unit;
    p_unit  = WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    p_wk->p_wait  = WBM_WAITICON_Init( p_unit, p_wk->p_res, HEAPID_WIFIBATTLEMATCH_CORE );
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
	}


  //�퓬���^���̏ꍇ�͊��Ƀv���C���[�����Ă񂾏�Ԃɂ���
  if( p_param->mode != WIFIBATTLEMATCH_CORE_MODE_START )
  { 
    if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      Util_PlayerInfo_CreateStay( p_wk, WIFIBATTLEMATCH_CORE_RETMODE_RATE );
    }
    else if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_FREE )
    { 
      if( p_param->mode == WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE )
      { 
        Util_SaveFreeScore( p_param->p_rndmatch, p_param->p_param->btl_rule, p_param->cp_btl_score->result );
      }
      Util_PlayerInfo_CreateStay( p_wk, WIFIBATTLEMATCH_CORE_RETMODE_FREE );
    }
  }

#ifdef DEBUGWIN_USE
  DEBUGWIN_InitProc( GFL_BG_FRAME0_M, p_wk->p_font );
  DEBUGWIN_ChangeLetterColor( 0,31,0 );
  DEBUGWIN_WIFISCORE_Init( HEAPID_WIFIBATTLEMATCH_CORE );
#endif

  WIFIBATTLEMATCH_NETICON_SetDraw( p_wk->p_net, TRUE );
	
	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`�����_���ΐ�	�R�A�v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_RND_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
	WIFIBATTLEMATCH_RND_WORK	      *p_wk	= p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_param_adrs;

  WIFIBATTLEMATCH_NETICON_SetDraw( p_wk->p_net, FALSE );

#ifdef DEBUGWIN_USE
  DEBUGWIN_WIFISCORE_Exit();
  DEBUGWIN_ExitProc();
#endif

	//���W���[���̔j��
  WBM_WAITICON_Exit( p_wk->p_wait );
  WBM_SEQ_Exit( p_wk->p_subseq );
  WBM_SEQ_Exit( p_wk->p_seq );
  WBM_TEXT_Exit( p_wk->p_text );
  if( p_wk->p_net )
  { 
    WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );
  }
  Util_MatchInfo_Delete( p_wk );
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
static GFL_PROC_RESULT WIFIBATTLEMATCH_RND_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_MAIN,
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
  };


	WIFIBATTLEMATCH_RND_WORK	*p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;

#ifdef PM_DEBUG
#if 0
  { 
    static u32  s_core_buff = 0xFFFFFFFF;
    static u32  s_sys_buff = 0xFFFFFFFF;
    static u32  s_app_buff = 0xFFFFFFFF;
    DEBUG_HEAP_PrintRestUse2( HEAPID_WIFIBATTLEMATCH_CORE, &s_core_buff );
    DEBUG_HEAP_PrintRestUse2( HEAPID_WIFIBATTLEMATCH_SYS, &s_sys_buff );
    DEBUG_HEAP_PrintRestUse2( GFL_HEAPID_APP, &s_app_buff );
  }
#endif
#endif

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
 *  �V�[�P���X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`������
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Init( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_RND_WORK	      *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  NAGI_Printf( " mode%d ret%d\n", p_param->mode, p_param->retmode );
  switch( p_param->mode )
  { 
  case WIFIBATTLEMATCH_CORE_MODE_START:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
    break;

  case WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR:
    WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE );
    /* fallthrough */

  case WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE:
    if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_EndBattle );
    }
    else if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_FREE )
    { 
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_EndBattle );
    }
    break;

  case WIFIBATTLEMATCH_CORE_MODE_ENDREC:
    if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_EndRec );
    }
    else if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_FREE )
    { 
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_EndRec );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`�J�n
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ���[�N
 *	@param	WBM_SEQ_WORK *p_wk          �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG_INIT,

    SEQ_START_RECV_GPF,
    SEQ_WAIT_RECV_GPF,

    SEQ_CHECK_GPF,

    SEQ_SELECT_MSG,
    SEQ_START_SELECT_MODE,
    SEQ_WAIT_SELECT_MODE,
    SEQ_START_CANCEL_MSG,
    SEQ_START_CANCEL_SELECT,
    SEQ_WAIT_CANCEL_SELECT,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	      *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_MSG_INIT:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
    *p_seq = SEQ_START_RECV_GPF;
    break;


  case SEQ_START_RECV_GPF:
    GFL_STD_MemClear( p_wk->p_param->p_gpf_data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA) );
    WIFIBATTLEMATCH_NET_StartRecvGpfData( p_wk->p_net, HEAPID_WIFIBATTLEMATCH_CORE );
    *p_seq = SEQ_WAIT_RECV_GPF;
    break;

  case SEQ_WAIT_RECV_GPF:
    switch( WIFIBATTLEMATCH_NET_WaitRecvGpfData( p_wk->p_net ) )
    { 
    case WIFIBATTLEMATCH_RECV_GPFDATA_RET_UPDATE:
      break;
    case WIFIBATTLEMATCH_RECV_GPFDATA_RET_SUCCESS:
      WIFIBATTLEMATCH_NET_GetRecvGpfData( p_wk->p_net, p_wk->p_param->p_gpf_data );
      *p_seq = SEQ_CHECK_GPF;
      break;
    case WIFIBATTLEMATCH_RECV_GPFDATA_RET_DIRTY:
      //���炦�Ȃ������̂ŁA�N���A
      GFL_STD_MemClear( p_wk->p_param->p_gpf_data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA) );
      *p_seq = SEQ_CHECK_GPF;
      break;
    case WIFIBATTLEMATCH_RECV_GPFDATA_RET_ERROR:
      //�G���[����
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT: //�ؒf�����O�C�������蒼��
        /* fallthrough */
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:     //���A�n�_�܂Ŗ߂�
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
        break;
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_FATAL:      //�d���ؒf
        NetErr_DispCallFatal();
        break;
      }
      break;
    }
    break;

    //-------------------------------------
    /// GPF�`�F�b�N
    //=====================================
  case SEQ_CHECK_GPF:

#ifdef DEBUG_GPF_PASS
    { 
      *p_seq = SEQ_SELECT_MSG;
    }
#else

#ifdef DEBUG_FLAG_GPF_PASS_ON
    if( DEBUG_FLG_GetFlg( DEBUG_FLG_WifiMatchRateFlag ) )
    { 
      *p_seq = SEQ_SELECT_MSG;
      return;
    }
#endif//DEBUG_FLAG_GPF_PASS_ON
    { 
      if( p_wk->p_param->p_gpf_data->signin )
      { 
        *p_seq = SEQ_SELECT_MSG;
      }
      else
      { 
        p_param->retmode = WIFIBATTLEMATCH_CORE_RETMODE_FREE;
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_Start );
        break;
      }
    }
#endif//DEBUG_GPF_PASS
    break;

    //-------------------------------------
    ///	���[�h�I��
    //=====================================
  case SEQ_SELECT_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_001, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SELECT_MODE );
    break;

  case SEQ_START_SELECT_MODE:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_FREERATE );
    *p_seq     = SEQ_WAIT_SELECT_MODE;
    break;

  case SEQ_WAIT_SELECT_MODE:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0:
          *p_seq = 0;
          p_param->retmode = WIFIBATTLEMATCH_CORE_RETMODE_FREE;
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_Start );
          break;
        case 1:
          *p_seq = 0;
          p_param->retmode = WIFIBATTLEMATCH_CORE_RETMODE_RATE;
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_Start );
          break;
        case 2:
          p_param->retmode = WIFIBATTLEMATCH_CORE_RETMODE_NONE;
          *p_seq = SEQ_START_CANCEL_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	��߂�
    //=====================================
  case SEQ_START_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_006, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_CANCEL_SELECT );
    break;

  case SEQ_START_CANCEL_SELECT:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq     = SEQ_WAIT_CANCEL_SELECT;
    break;

  case SEQ_WAIT_CANCEL_SELECT:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0:
          p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_DisConnectEnd );
          break;

        case 1:
          *p_seq = SEQ_SELECT_MSG;
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
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  };
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  ���[�e�B���O�J�n
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_RATE_MSG,
    SEQ_START_RECV_MSG,
    SEQ_WAIT_RECVRATE_SAKE,
    SEQ_WAIT_CARDIN,
    SEQ_START_POKECHECK_SERVER,
    SEQ_WAIT_POKECHECK_SERVER,
    SEQ_START_DIRTY_MSG,
    SEQ_DIRTY_END,
    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	      *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_RATE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_018, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_RECV_MSG );
    break;


  case SEQ_START_RECV_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_POKECHECK_SERVER );
    break;

    //-------------------------------------
    /// �|�P�����s���`�F�b�N
    //=====================================
  case SEQ_START_POKECHECK_SERVER:
    Util_SubSeq_Start( p_wk, WbmRndSubSeq_EvilCheck );
    *p_seq       = SEQ_WAIT_POKECHECK_SERVER;
    break;

  case SEQ_WAIT_POKECHECK_SERVER:
    { 
      WBM_RND_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_RND_SUBSEQ_EVILCHECK_RET_SUCCESS )
      { 
        *p_seq       = SEQ_END;
      }
      else if( ret == WBM_RND_SUBSEQ_EVILCHECK_RET_DARTY )
      {   
        *p_seq       = SEQ_START_DIRTY_MSG;
      }

      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
        break;
      }
    }
    break;
    
  case SEQ_START_DIRTY_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_ERR_00, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    break;

  case SEQ_DIRTY_END:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_DisConnectEnd );
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_StartMatching );
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
 *	@brief  �����_���}�b�`���[�e�B���O    �}�b�`���O�O��SAKE�T�[�o�[��M
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_StartMatching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_RECVRATE_MSG,
    SEQ_START_RECVRATE_SAKE,
    SEQ_WAIT_RECVRATE_SAKE,

    SEQ_START_CREATE_SAKE,
    SEQ_WAIT_CREATE_SAKE,

    SEQ_START_CARDIN,
    SEQ_WAIT_CARDIN,
    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	      *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;


  switch( *p_seq )
  { 
    //-------------------------------------
    ///	���[�e�B���O�f�[�^��M
    //=====================================
  case SEQ_START_RECVRATE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_RECVRATE_SAKE );
    *p_seq       = SEQ_WAIT_MSG;
    break;

  case SEQ_START_RECVRATE_SAKE:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, WIFIBATTLEMATCH_GDB_MYRECORD, WIFIBATTLEMATCH_GDB_GET_RND_SCORE, p_wk->p_param->p_rnd_sake_data );
      *p_seq       = SEQ_WAIT_RECVRATE_SAKE;
    }
    OS_TPrintf( "�T�[�o�[�҂�%d\n",  *p_wk->p_param->p_server_time );
    break;
    
  case SEQ_WAIT_RECVRATE_SAKE:
    { 
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_GDB_Process( p_wk->p_net );

      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 

        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;

        //���P��M���Ă����f�[�^���Z�[�u���[�N�ɓ����
        Util_SaveRateScore( p_param->p_rndmatch, p_param->p_param->btl_rule, p_wk );
        Util_RenewalMyData( p_param->p_player_data, p_wk );

        //���������Ȃ������풓�ɕۑ�
        p_wk->p_param->p_recv_data->record_save_idx = p_wk->p_param->p_rnd_sake_data->record_save_idx;

        *p_seq       = SEQ_START_CREATE_SAKE;
      }
      else if( res == WIFIBATTLEMATCH_GDB_RESULT_ERROR )
      { 
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );

          *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
          *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	���R�[�hID���Ȃ��ꍇ�̃��R�[�h�쐬����
    //=====================================
  case SEQ_START_CREATE_SAKE:
    if( WIFIBATTLEMATCH_GDB_GetRecordID( p_wk->p_net ) == 0 )
    {
      WIFIBATTLEMATCH_GDB_StartCreateRecord( p_wk->p_net );
      *p_seq      = SEQ_WAIT_CREATE_SAKE;
    }
    else
    {
      *p_seq       = SEQ_START_CARDIN;
    }
    break;

  case SEQ_WAIT_CREATE_SAKE:
    {
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_GDB_ProcessCreateRecord( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      {
        //������x��M���ɍs��
        *p_seq  = SEQ_START_RECVRATE_SAKE;
      }
      else if( res == WIFIBATTLEMATCH_GDB_RESULT_ERROR )
      {
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );

          *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
          *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	�����̃J�[�h����
    //=====================================
  case SEQ_START_CARDIN:
    //�����̏���\��
    Util_PlayerInfo_Create( p_wk, WIFIBATTLEMATCH_CORE_RETMODE_RATE );
    *p_seq       = SEQ_WAIT_CARDIN;
    break;

  case SEQ_WAIT_CARDIN:
    if( Util_PlayerInfo_Move( p_wk ) )
    { 
      *p_seq       = SEQ_END;
    }
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_Matching );
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
 *	@brief  �����_���}�b�`  ���[�e�B���O�̃}�b�`���O
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MATCHING_MSG,
    SEQ_START_MATCHING,
    SEQ_WAIT_MATCHING,
    SEQ_START_SENDDATA,
    SEQ_WAIT_SENDDATA,
    SEQ_CHECK_DARTYDATA,
    SEQ_START_BADWORD,
    SEQ_WAIT_BADWORD,

    SEQ_START_CANCEL_TIMING,
    SEQ_WAIT_CANCEL_TIMING,
    SEQ_START_OK_TIMING,
    SEQ_WAIT_OK_TIMING,

    SEQ_OK_MATCHING_MSG,
    SEQ_OK_MATCHING_WAIT,
    SEQ_WAIT_MATCH_CARDIN,
    SEQ_CARDIN_WAIT,

    SEQ_START_SESSION,
    SEQ_WAIT_SESSION,

    SEQ_END_SESSION,
    SEQ_END_MATCHING_MSG,
    SEQ_END_MATCHING,

    //�L�����Z������
    //�����̑ΐ�������ɂ��܂�
    SEQ_START_SELECT_END_MSG,
    SEQ_START_END_LIST,
    SEQ_SELECT_END_LIST,

    //����������Â��܂����H
    SEQ_START_SELECT_CONTINUE_MSG,
    SEQ_START_CONTINUE_LIST,
    SEQ_SELECT_CONTINUE_LIST,

    SEQ_START_CANCEL,
    SEQ_END_CANCEL,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	      *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;
  UTIL_CANCEL_STATE cancel_state;
  BOOL is_timeout_enable  = FALSE;

  //-------------------------------------
  ///	�L�����Z���V�[�P���X
  //=====================================
  {
    BOOL is_cancel_enable = (SEQ_WAIT_MATCHING <= *p_seq && *p_seq < SEQ_START_CANCEL_TIMING);
    cancel_state  = Util_Cancel_Seq( p_wk, is_cancel_enable );

    //�������ؒf
    if( cancel_state == UTIL_CANCEL_STATE_DECIDE 
        && (*p_seq != SEQ_START_BADWORD && *p_seq != SEQ_WAIT_BADWORD ) )
    {
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE );
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
    ///	�}�b�`���O�J�n
    //=====================================
  case SEQ_START_MATCHING_MSG:
    p_wk->cancel_seq  = 0;
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, TRUE );
    PMSND_PlaySE( WBM_SND_SE_MATCHING );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_009, WBM_TEXT_TYPE_QUE );
    *p_seq = SEQ_START_MATCHING;
    break;

  case SEQ_START_MATCHING:
    { 
      WIFIBATTLEMATCH_MATCH_KEY_DATA  data;
      Util_Matchkey_SetData( &data, p_wk );
      WIFIBATTLEMATCH_NET_StartMatchMake( p_wk->p_net, WIFIBATTLEMATCH_TYPE_RNDRATE, TRUE, p_param->p_param->btl_rule, &data );
      *p_seq = SEQ_WAIT_MATCHING;
    }
    break;

  case SEQ_WAIT_MATCHING:
    //�}�b�`���O�G���[
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
      PMSND_StopSE();
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
      return;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
      PMSND_StopSE();
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      return ;
    }

    //�}�b�`���O�L�����Z������
    {
      WIFIBATTLEMATCH_NET_MATCHMAKE_STATE state = WIFIBATTLEMATCH_NET_WaitMatchMake( p_wk->p_net );
      //�}�b�`���O�I���҂�
      if( WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_SUCCESS == state )
      { 
        p_wk->match_timeout = 0;
        *p_seq = SEQ_START_SENDDATA;
      }
      else if( WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_FAILED == state )
      {
        *p_seq = SEQ_START_MATCHING;
      }
    }

    break;
  case SEQ_START_SENDDATA:
    if( cancel_state == UTIL_CANCEL_STATE_NONE )
    { 
      WIFIBATTLEMATCH_ENEMYDATA *p_my_data = p_param->p_player_data;
      if( WIFIBATTLEMATCH_NET_StartEnemyData( p_wk->p_net, p_my_data ) )
      { 
        p_wk->match_timeout = 0;
        *p_seq       = SEQ_WAIT_SENDDATA;
      }
      is_timeout_enable = TRUE;
    }
    break;

  case SEQ_WAIT_SENDDATA:
    {
      WIFIBATTLEMATCH_ENEMYDATA *p_recv;
      if( WIFIBATTLEMATCH_NET_WaitEnemyData( p_wk->p_net, &p_recv ) )
      {   
        u32 dirty;
        WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data = p_param->p_enemy_data;
        GFL_STD_MemCopy( p_recv, p_enemy_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
        dirty = WIFIBATTLEMATCH_DATA_ModifiEnemyData( p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );

        if( dirty == 0 )
        { 
          SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
          WIFI_NEGOTIATION_SAVEDATA* pSV  = WIFI_NEGOTIATION_SV_GetSaveData(p_sv_ctrl);
//          WIFI_NEGOTIATION_SV_SetFriend(pSV, (MYSTATUS*)p_param->p_enemy_data->mystatus );
          //�d�l���Ȃ��Ȃ�܂���
        }

        p_wk->match_timeout = 0;
        *p_seq  = SEQ_CHECK_DARTYDATA;
      }
      is_timeout_enable = TRUE;
    }
    break;

  case SEQ_CHECK_DARTYDATA:
#ifdef DEBUG_DIRTYCHECK_PASS
    if( 1 )
#else
    if( Util_VerifyPokeData( p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE ) )
#endif
    { 
      //�ΐ�ҏ��\��
      *p_seq  = SEQ_START_BADWORD;
    }
    else
    { 
      //�s���Ȃ̂Ń}�b�`���O�ɖ߂�
      WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_Matching );
    }
    break;

  case SEQ_START_BADWORD:
    { 
      WIFIBATTLEMATCH_NET_StartBadWord( p_wk->p_net, (MYSTATUS*)p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );
    }
    *p_seq  = SEQ_WAIT_BADWORD;
    break;
  case SEQ_WAIT_BADWORD:
    { 
      BOOL ret;
      BOOL is_badword;
      
      ret = WIFIBATTLEMATCH_NET_WaitBadWord( p_wk->p_net, &is_badword );
      if( ret )
      { 
        *p_seq  = SEQ_START_CANCEL_TIMING;
      }
      else
      { 
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_CupContinue );
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
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
      *p_seq  = SEQ_OK_MATCHING_MSG;
    }
    break;


  case SEQ_OK_MATCHING_MSG:
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
    PMSND_StopSE();
    PMSND_PlaySE( WBM_SND_SE_MATCHING_OK );

    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_010, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_OK_MATCHING_WAIT );
    break;

  case SEQ_OK_MATCHING_WAIT:
    if( p_wk->cnt++ > MATCHING_MSG_WAIT_SYNC )
    { 
      p_wk->cnt      = 0;
      Util_MatchInfo_Create( p_wk, p_param->p_enemy_data );
      GFL_BG_SetVisible( BG_FRAME_M_TEXT, FALSE );
      *p_seq      = SEQ_WAIT_MATCH_CARDIN;
    }
    break;
  case SEQ_WAIT_MATCH_CARDIN:
    if( Util_MatchInfo_Main( p_wk ) )
    { 
      *p_seq  = SEQ_CARDIN_WAIT;
    }
    break;
  case SEQ_CARDIN_WAIT:
    if( p_wk->cnt++ > ENEMYDATA_WAIT_SYNC_RATE )
    { 
      p_wk->cnt      = 0;
      *p_seq      = SEQ_START_SESSION;
    }
    break;

  case SEQ_START_SESSION:
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_SC, WBM_SC_HEAP_SIZE );
    DWC_RAPCOMMON_SetSubHeapID( DWC_ALLOCTYPE_GS, WBM_SC_HEAP_SIZE, HEAPID_WIFIBATTLEMATCH_SC );

    WIFIBATTLEMATCH_SC_StartReport( p_wk->p_net, WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_AFTER, WIFIBATTLEMATCH_TYPE_RNDRATE, 0, NULL, FALSE );
    *p_seq  = SEQ_WAIT_SESSION;
    break;

  case SEQ_WAIT_SESSION:
    {
      WIFIBATTLEMATCH_NET_SC_STATE  state = WIFIBATTLEMATCH_SC_ProcessReport(p_wk->p_net );
      if( state == WIFIBATTLEMATCH_NET_SC_STATE_SUCCESS )
      { 
        *p_seq  = SEQ_END_SESSION;
      }
      
      if( state != WIFIBATTLEMATCH_NET_SC_STATE_UPDATE )
      {
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:     //�߂�
          DWC_RAPCOMMON_ResetSubHeapID();
          GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SC );

          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_CupContinue );
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          DWC_RAPCOMMON_ResetSubHeapID();
          GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SC );

          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

  case SEQ_END_SESSION:
    DWC_RAPCOMMON_ResetSubHeapID();
    GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SC );
    *p_seq  = SEQ_END_MATCHING_MSG;
    break;

  case SEQ_END_MATCHING_MSG:
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

    //-------------------------------------
    ///	�L�����Z������
    //=====================================


    //�����_���}�b�`�������ɂ��܂�
  case SEQ_START_SELECT_END_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_016, WBM_TEXT_TYPE_STREAM );
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
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_DisConnextSendTime );
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
          *p_seq  = SEQ_START_MATCHING_MSG;
          break;
        case 1: //������
          *p_seq  = SEQ_START_SELECT_END_MSG;
          break;
        }
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
    WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE );
    *p_seq  = SEQ_START_MATCHING_MSG;
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
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  ���[�e�B���O�̐퓬�I����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
 
  enum
  { 
    SEQ_SC_HEAP_INIT,
    SEQ_START_MSG,
    SEQ_START_REPORT_ATLAS,
    SEQ_WAIT_REPORT_ATLAS,

    SEQ_SC_HEAP_EXIT,

    SEQ_START_DISCONNECT,
    SEQ_WAIT_DISCONNECT,

    SEQ_START_MSG_WAIT,
    SEQ_START_SAKE_RECORD,
    SEQ_WAIT_SAKE_RECORD,

    SEQ_START_RESULT_MSG,
    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,



    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_SC_HEAP_INIT:
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_SC, WBM_SC_HEAP_SIZE );
    DWC_RAPCOMMON_SetSubHeapID( DWC_ALLOCTYPE_GS, WBM_SC_HEAP_SIZE, HEAPID_WIFIBATTLEMATCH_SC );
    *p_seq  = SEQ_START_MSG;
    break;

    //-------------------------------------
    ///	Atlas�ւ̏�������
    //=====================================
  case SEQ_START_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_013, WBM_TEXT_TYPE_WAIT);
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_REPORT_ATLAS );
    break;

  case SEQ_START_REPORT_ATLAS:
    {
      BOOL is_error = p_param->mode == WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;
      WIFIBATTLEMATCH_SC_StartReport( p_wk->p_net, WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_SCORE, WIFIBATTLEMATCH_TYPE_RNDRATE, p_param->p_param->btl_rule, p_param->cp_btl_score, is_error );
    }
    *p_seq = SEQ_WAIT_REPORT_ATLAS;
    break;
  case SEQ_WAIT_REPORT_ATLAS:
    { 
      WIFIBATTLEMATCH_NET_SC_STATE  state = WIFIBATTLEMATCH_SC_ProcessReport(p_wk->p_net );
      if( state == WIFIBATTLEMATCH_NET_SC_STATE_SUCCESS )
      { 
        *p_seq = SEQ_SC_HEAP_EXIT;
      }

      if( state != WIFIBATTLEMATCH_NET_SC_STATE_UPDATE )
      {
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:     //�߂�
          *p_seq = SEQ_START_SAVE_MSG;
          DWC_RAPCOMMON_ResetSubHeapID();
          GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SC );
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
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
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_EndRec );
    }
    else
    { 
      *p_seq = SEQ_START_DISCONNECT;
    }
    break;

    //-------------------------------------
    /// �ؒf����
    //=====================================
  case SEQ_START_DISCONNECT:
    
    //�����܂Őؒf�����Ȃ��ꍇ�͑����m�荇���ɓo�^
    {           
      SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      ETC_SAVE_WORK *p_etc  = SaveData_GetEtc( p_sv_ctrl );
      WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data = p_param->p_enemy_data;
      EtcSave_SetAcquaintance( p_etc, MyStatus_GetID( (MYSTATUS*)p_enemy_data->mystatus ) );
    }
    *p_seq = SEQ_WAIT_DISCONNECT;
    break;

  case SEQ_WAIT_DISCONNECT:
    if( WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE ) )
    {
      if( *p_wk->p_param->p_server_time == 0 )
      { 
        *p_seq  = SEQ_START_SAKE_RECORD;
      }
      else
      { 
        WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_019, WBM_TEXT_TYPE_WAIT );
        *p_seq = SEQ_WAIT_MSG;
        WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_MSG_WAIT );
      }
    }
    break;


  case SEQ_START_MSG_WAIT:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      OS_TPrintf( "�T�[�o�[�҂�%d\n",  *p_wk->p_param->p_server_time );
      *p_seq  = SEQ_START_SAKE_RECORD;
    }
    break;

  case SEQ_START_SAKE_RECORD:
    //����������I
    { 
      WIFIBATTLEMATCH_GDB_RND_RECORD_DATA data;
      GFL_STD_MemClear( &data, sizeof(WIFIBATTLEMATCH_GDB_RND_RECORD_DATA) );
      data.record_data  = *p_wk->p_param->p_record_data;
      data.save_idx     = p_wk->p_param->p_recv_data->record_save_idx;
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_RECORD, &data );
    }
    *p_seq  = SEQ_WAIT_SAKE_RECORD;
    break;

  case SEQ_WAIT_SAKE_RECORD:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );

      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq = SEQ_START_RESULT_MSG;
      }
      else if( res == WIFIBATTLEMATCH_GDB_RESULT_ERROR )
      { 
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE  ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
          *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
          *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
          *p_seq = SEQ_START_RESULT_MSG;
          break;
        }
      }
    } 
    break;

  case SEQ_START_RESULT_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_33, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE_MSG );
    break;

    //-------------------------------------
    /// ���[�e�B���O�Z�[�u����
    //=====================================
  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, WBM_TEXT_TYPE_WAIT );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    break;
  case SEQ_START_SAVE:
    //���[�e�B���O�ŃZ�[�u�f�[�^�ɕۑ�������̂́A�����̃X�R�A�̂�
    //�}�b�`���O�O�ŃZ�[�u���[�N�ɑ�������Ă���
    GAMEDATA_SaveAsyncStart(p_param->p_param->p_game_data);
    *p_seq       = SEQ_WAIT_SAVE;
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
        p_param->result = WIFIBATTLEMATCH_CORE_RESULT_NEXT_REC;
        WBM_SEQ_End( p_seqwk ); 
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
 *	@brief  �����_���}�b�`  ���[�e�B���O���[�h�^�悩��̖߂�
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_EndRec( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_CupContinue );
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
 *	@brief  �����_���}�b�`  ��������𑱂��܂����H
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_CupContinue( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_SELECT_CONTINUE_MSG,
    SEQ_START_SELECT_CONTINUE,
    SEQ_WAIT_SELECT_CONTINUE,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
    //-------------------------------------
    /// �ΐ푱�s�m�F
    //=====================================
  case SEQ_START_SELECT_CONTINUE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_015, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SELECT_CONTINUE );
    break;
  case SEQ_START_SELECT_CONTINUE:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
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
        case 0:
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_StartMatching );
          break;
        case 1:
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_CupEnd );
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
      WBM_SEQ_NextReservSeq( p_seqwk ); 
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  �����������I�����܂����H
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_SELECT_CANCEL_MSG,
    SEQ_START_SELECT_CANCEL,
    SEQ_WAIT_SELECT_CANCEL,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
    //-------------------------------------
    /// �����_���}�b�`�I���m�F
    //=====================================
  case SEQ_START_SELECT_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_016, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SELECT_CANCEL );
    break;

  case SEQ_START_SELECT_CANCEL:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq     = SEQ_WAIT_SELECT_CANCEL;
    break;
  case SEQ_WAIT_SELECT_CANCEL:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0:
          WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE );

          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_DisConnextSendTime );
          break;
        case 1:
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_CupContinue );
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
      WBM_SEQ_NextReservSeq( p_seqwk ); 
    }
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  ���[�e�B���O���[�h  �T�P�ɓ����𑗐M���ďI��
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_DisConnextSendTime( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG_SAKE_TIME,
    SEQ_START_SEND_SAKE_TIME,
    SEQ_WAIT_SEND_SAKE_TIME,

    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
    //-------------------------------------
    /// �T�P�ւ̓�����������
    //=====================================
  case SEQ_START_MSG_SAKE_TIME:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_013, WBM_TEXT_TYPE_WAIT );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SEND_SAKE_TIME );
    break;

  case SEQ_START_SEND_SAKE_TIME:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_MYINFO, NULL );
      *p_seq  = SEQ_WAIT_SEND_SAKE_TIME;
    }
    else
    { 
      OS_TPrintf( "�T�[�o�[�҂�%d\n",  *p_wk->p_param->p_server_time );
    }
    break;

  case SEQ_WAIT_SEND_SAKE_TIME:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );
      //�G���[�����낤�ƂȂ��낤�ƏI���ւ���
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_END;
      }
    }
    break;

  case SEQ_END:
    WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE );
    p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_DisConnectEnd );
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
//=============================================================================
/**
 *      �t���[���[�h
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  �t���[���[�h�J�n
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Free_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_FREE_MSG,
    SEQ_WAIT_CARDIN,
    SEQ_WAIT_MSG,

    SEQ_START_MSG_EVILCHECK,

    SEQ_START_SUBSEQ_EVILCHECK,
    SEQ_WAIT_SUBSEQ_EVILCHECK,
    SEQ_START_DIRTY_MSG,
    SEQ_DIRTY_END,
  };

  WIFIBATTLEMATCH_RND_WORK	      *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
    //-------------------------------------
    ///	�t���[���[�h�J�n���b�Z�[�W
    //=====================================
  case SEQ_START_FREE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_002, WBM_TEXT_TYPE_STREAM );
    { 
      //�����̃f�[�^��\��
      Util_PlayerInfo_Create( p_wk, WIFIBATTLEMATCH_CORE_RETMODE_FREE );
    }
    *p_seq       = SEQ_WAIT_CARDIN;
    break;

  case SEQ_WAIT_CARDIN:
    if( Util_PlayerInfo_Move( p_wk ) )
    { 
      *p_seq       = SEQ_WAIT_MSG;
    }
    break;

  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    {
#ifdef DEBUG_DIRTYCHECK_PASS
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_Matching );
#else
      *p_seq  = SEQ_START_MSG_EVILCHECK;
#endif 
    }
    break;

    //-------------------------------------
    /// �s���`�F�b�N
    //=====================================

  case SEQ_START_MSG_EVILCHECK:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
    *p_seq  = SEQ_START_SUBSEQ_EVILCHECK;
    break;

  case SEQ_START_SUBSEQ_EVILCHECK:
    Util_SubSeq_Start( p_wk, WbmRndSubSeq_EvilCheck );
    *p_seq  = SEQ_WAIT_SUBSEQ_EVILCHECK;
    break;

  case SEQ_WAIT_SUBSEQ_EVILCHECK:
    { 
      WBM_RND_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_RND_SUBSEQ_EVILCHECK_RET_SUCCESS )
      { 
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_Matching );
      }
      else if( ret == WBM_RND_SUBSEQ_EVILCHECK_RET_DARTY )
      {   
        *p_seq       = SEQ_START_DIRTY_MSG;
      }

      //�G���[
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
        break;
      }
    }
    break;
    
  case SEQ_START_DIRTY_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_ERR_00, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_DIRTY_END;
    break;

  case SEQ_DIRTY_END:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    {
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_DisConnectEnd );
    }
    break;

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  �t���[���[�h�̃}�b�`���O����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 *
 *  �ڑ�OK�ƕ\������邬�肬��܂ŁA�͂��A�������ŃL�����Z�������邽�߁A�ȉ��̂��Ƃ��s���Ă���
 *  �E�͂�/�������I�𒆂͐ڑ�OK�܂ŃV�[�P���X�������Ȃ����Ƃ��l�����āA�e�҂������Ń^�C���A�E�g��ݒ�
 *  �E�͂����������Ƃ���A�^�C���A�E�g�Őؒf����Ƃ���
 * �@�@ ����ƌq�����Ă�����ؒf�L�����Z���t���O�𑗐M���Ă���ؒf
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Free_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_START_MATCHING_MSG,
    SEQ_START_MATCHING,
    SEQ_WAIT_MATCHING,
    SEQ_START_CHECK_CHEAT,
    SEQ_WAIT_CHECK_CHEAT,
    SEQ_START_SENDDATA,
    SEQ_WAIT_SENDDATA,

    SEQ_CHECK_DARTYDATA,
    SEQ_START_BADWORD,
    SEQ_WAIT_BADWORD,

    SEQ_START_CANCEL_TIMING,
    SEQ_WAIT_CANCEL_TIMING,
    SEQ_START_OK_TIMING,
    SEQ_WAIT_OK_TIMING,

    SEQ_OK_MATCHING_MSG,
    SEQ_OK_MATCHING_WAIT,
    SEQ_WAIT_MATCH_CARDIN,
    SEQ_CARDIN_WAIT,
    SEQ_END_MATCHING_MSG,
    SEQ_END_MATCHING,

    SEQ_START_CANCEL,
    SEQ_END_CANCEL,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;
  UTIL_CANCEL_STATE cancel_state;
  BOOL is_timeout_enable  = FALSE;

  //-------------------------------------
  ///	�L�����Z���V�[�P���X
  //=====================================
  {
    BOOL is_cancel_enable = (SEQ_WAIT_MATCHING <= *p_seq && *p_seq < SEQ_START_CANCEL_TIMING);
    cancel_state  = Util_Cancel_Seq( p_wk, is_cancel_enable );

    //�������ؒf
    if( cancel_state == UTIL_CANCEL_STATE_DECIDE 
        && (*p_seq != SEQ_START_BADWORD && *p_seq != SEQ_WAIT_BADWORD ) )
    {
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_CupEnd );
      return ;
    }
  }

  //-------------------------------------
  ///	�}�b�`���O�V�[�P���X
  //=====================================
  switch( *p_seq )
  { 
    //-------------------------------------
    ///	�}�b�`���O�J�n
    //=====================================
  case SEQ_INIT:
    { 
      Util_RenewalMyData( p_param->p_player_data, p_wk );
      *p_seq = SEQ_START_MATCHING_MSG;
    }
    break;

  case SEQ_START_MATCHING_MSG:
    p_wk->cancel_seq  = 0;
    PMSND_PlaySE( WBM_SND_SE_MATCHING );
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, TRUE );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_009, WBM_TEXT_TYPE_QUE );
    *p_seq = SEQ_START_MATCHING;
    break;

  case SEQ_START_MATCHING:
    { 
      WIFIBATTLEMATCH_MATCH_KEY_DATA  data;
      Util_Matchkey_SetData( &data, p_wk );
      WIFIBATTLEMATCH_NET_StartMatchMake( p_wk->p_net, WIFIBATTLEMATCH_TYPE_RNDFREE, FALSE, p_param->p_param->btl_rule, &data ); 
    }
    *p_seq  = SEQ_WAIT_MATCHING;
    break;

  case SEQ_WAIT_MATCHING:
    //�}�b�`���O�G���[
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
      PMSND_StopSE();
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
      return;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
      PMSND_StopSE();
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      return;
    }

    {
      WIFIBATTLEMATCH_NET_MATCHMAKE_STATE state = WIFIBATTLEMATCH_NET_WaitMatchMake( p_wk->p_net );
      //�}�b�`���O�I���҂�
      if( WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_SUCCESS == state )
      { 
        p_wk->match_timeout = 0;
        *p_seq = SEQ_START_SENDDATA;
      }
      else if( WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_FAILED == state )
      {
        *p_seq = SEQ_START_MATCHING;
      }
    }
    break;

    //-------------------------------------
    ///	�f�[�^����M
    //=====================================
  case SEQ_START_SENDDATA:
    //�L�����Z����Ԃ̂Ƃ��͑��M���Ȃ�
    if( cancel_state == UTIL_CANCEL_STATE_NONE )
    {
      WIFIBATTLEMATCH_ENEMYDATA *p_my_data = p_param->p_player_data;
      if( WIFIBATTLEMATCH_NET_StartEnemyData( p_wk->p_net, p_my_data ) )
      { 
        p_wk->match_timeout = 0;
        *p_seq       = SEQ_WAIT_SENDDATA;
      }
      is_timeout_enable = TRUE;
    }
    break;

  case SEQ_WAIT_SENDDATA:
    {
      WIFIBATTLEMATCH_ENEMYDATA *p_recv;
      if( WIFIBATTLEMATCH_NET_WaitEnemyData( p_wk->p_net, &p_recv ) )
      { 
        u32 dirty;
        WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data = p_param->p_enemy_data;
        GFL_STD_MemCopy( p_recv, p_enemy_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
  

        dirty = WIFIBATTLEMATCH_DATA_ModifiEnemyData( p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );

        p_wk->match_timeout = 0;
        *p_seq       = SEQ_CHECK_DARTYDATA;
      }
      is_timeout_enable = TRUE;
    }
    break;

  case SEQ_CHECK_DARTYDATA:
#ifdef DEBUG_DIRTYCHECK_PASS
    if( 1 )
#else
    if( Util_VerifyPokeData( p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE ) )
#endif
    { 
      *p_seq       = SEQ_START_BADWORD;
    }
    else
    { 
      //�s���Ȃ̂Ń}�b�`���O�ɖ߂�
      WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_Matching );
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
        *p_seq  = SEQ_START_CANCEL_TIMING;
      }
      else
      { 
        //�G���[
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //�߂�
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_CupContinue );
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
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
      *p_seq  = SEQ_OK_MATCHING_MSG;
    }
    break;

  case SEQ_OK_MATCHING_MSG:
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
    PMSND_StopSE();
    PMSND_PlaySE( WBM_SND_SE_MATCHING_OK );

    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_010, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_OK_MATCHING_WAIT );
    break;

  case SEQ_OK_MATCHING_WAIT:
    if( p_wk->cnt++ > MATCHING_MSG_WAIT_SYNC )
    { 
      p_wk->cnt      = 0;
      Util_MatchInfo_Create( p_wk, p_param->p_enemy_data );
      GFL_BG_SetVisible( BG_FRAME_M_TEXT, FALSE );
      *p_seq      = SEQ_WAIT_MATCH_CARDIN;
    }
    break;

  case SEQ_WAIT_MATCH_CARDIN:
    if( Util_MatchInfo_Main( p_wk ) )
    { 
      *p_seq  = SEQ_CARDIN_WAIT;
    }
    break;

  case SEQ_CARDIN_WAIT:
    if( p_wk->cnt++ > ENEMYDATA_WAIT_SYNC )
    { 
      p_wk->cnt      = 0;
      *p_seq      = SEQ_END_MATCHING_MSG;
    }
    break;

  case SEQ_END_MATCHING_MSG:
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

  case SEQ_START_CANCEL:
    if( WIFIBATTLEMATCH_NET_SendMatchCancel( p_wk->p_net ) )
    {
      *p_seq  = SEQ_END_CANCEL;
    }
    break;

  case SEQ_END_CANCEL:
    WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE );
    *p_seq  = SEQ_START_MATCHING_MSG;
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
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_CupContinue );
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
        break;
      }
    }
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  �t���[���[�h�̐퓬�I����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Free_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK,
    SEQ_START_DISCONNECT,
    SEQ_WAIT_DISCONNECT,

    SEQ_START_CARD,
    SEQ_WAIT_CARD,

    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECK:
    //����ɐؒf����Ă�����A�^����΂�
    if( p_param->mode == WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR )
    { 
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_EndRec );
    }
    else
    { 
      *p_seq = SEQ_START_DISCONNECT;
    }
    break;

    //-------------------------------------
    /// �ؒf����
    //=====================================
  case SEQ_START_DISCONNECT:
    {           
      SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      ETC_SAVE_WORK *p_etc  = SaveData_GetEtc( p_sv_ctrl );
      WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data = p_param->p_enemy_data;
      EtcSave_SetAcquaintance( p_etc, MyStatus_GetID( (MYSTATUS*)p_enemy_data->mystatus ) );
    }
    *p_seq = SEQ_WAIT_DISCONNECT;
    break;

  case SEQ_WAIT_DISCONNECT:
    if( WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE ) )
    { 
      //�X�R�A���Z�[�u
      
      *p_seq  = SEQ_START_CARD;
    }
    break;

  case SEQ_START_CARD:
    //Util_PlayerInfo_Create( p_wk, WIFIBATTLEMATCH_CORE_RETMODE_FREE );
    *p_seq  = SEQ_WAIT_CARD;
    break;

  case SEQ_WAIT_CARD:
    //if( Util_PlayerInfo_Move( p_wk ) )
    { 
      *p_seq  = SEQ_START_SAVE_MSG;
    }
    break;

    //-------------------------------------
    /// �Z�[�u����
    //=====================================
  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, WBM_TEXT_TYPE_WAIT);
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    break;

  case SEQ_START_SAVE:
    GAMEDATA_SaveAsyncStart(p_param->p_param->p_game_data);
    *p_seq       = SEQ_WAIT_SAVE;
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
        p_param->result = WIFIBATTLEMATCH_CORE_RESULT_NEXT_REC;
        WBM_SEQ_End( p_seqwk ); 
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
 *	@brief  �����_���}�b�`  �t���[���[�h�̘^��I����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Free_EndRec( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_CARD,
    SEQ_WAIT_CARD,

    SEQ_START,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_CARD:
    //Util_PlayerInfo_Create( p_wk, WIFIBATTLEMATCH_CORE_RETMODE_FREE );
    *p_seq  = SEQ_WAIT_CARD;
    break;

  case SEQ_WAIT_CARD:
    //if( Util_PlayerInfo_Move( p_wk ) )
    { 
      *p_seq  = SEQ_START;
    }
    break;

  case SEQ_START:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_CupContinue );
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
 *	@brief  �����_���}�b�`  ����������Â��܂����H
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Free_CupContinue( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_SELECT_CONTINUE_MSG,
    SEQ_START_SELECT_CONTINUE,
    SEQ_WAIT_SELECT_CONTINUE,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
    //-------------------------------------
    /// �ΐ푱�s�m�F
    //=====================================
  case SEQ_START_SELECT_CONTINUE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_015, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SELECT_CONTINUE );
    break;
  case SEQ_START_SELECT_CONTINUE:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
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
        case 0:
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_Matching );
          break;
        case 1:
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_CupEnd );
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
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  �������������イ��傤���܂����H
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Free_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_SELECT_CANCEL_MSG,
    SEQ_START_SELECT_CANCEL,
    SEQ_WAIT_SELECT_CANCEL,

    SEQ_START_MSG_END,
    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
    //-------------------------------------
    /// �����_���}�b�`�I���m�F
    //=====================================
  case SEQ_START_SELECT_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_016, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SELECT_CANCEL );
    break;
  case SEQ_START_SELECT_CANCEL:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq     = SEQ_WAIT_SELECT_CANCEL;
    break;
  case SEQ_WAIT_SELECT_CANCEL:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0:
          p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
          *p_seq  = SEQ_START_MSG_END;
          break;
        case 1:
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_CupContinue );
          break;
        }
      }
    }
    break;

  case SEQ_START_MSG_END:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_020, WBM_TEXT_TYPE_WAIT );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_END );
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_DisConnectEnd );
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
 *	@brief  �G���[�̂��߃��O�C���ɖ߂�
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Err_ReturnLogin( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  p_param->result = WIFIBATTLEMATCH_CORE_RESULT_ERR_NEXT_LOGIN;
  WBM_SEQ_End( p_seqwk );
  
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ؒf���ďI��
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_DisConnectEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_END,
  };
  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_END:


    if( p_wk->p_net )
    { 
      WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );
      p_wk->p_net = NULL;
    }

    NetErr_ExitNetSystem();

    WBM_SEQ_End( p_seqwk );
    break;
  }
}
//=============================================================================
/**
 *    �T�u�V�[�P���X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �ؒf���ďI��
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmRndSubSeq_EvilCheck( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START,
    SEQ_START_EVIL_CHECK,
    SEQ_WAIT_EVIL_CHECK,
    SEQ_END_CHECK,
    SEQ_END,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	      = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param    = p_wk->p_param;
  WIFIBATTLEMATCH_ENEMYDATA   *p_my_data  = p_param->p_player_data;

  switch( *p_seq )
  { 
  case SEQ_START:
    p_wk->cnt = 0;
    GFL_STD_MemClear( &p_wk->evilecheck_data, sizeof(WIFIBATTLEMATCH_NET_EVILCHECK_DATA ));
    (*p_seq)++;
    break;

  case SEQ_START_EVIL_CHECK:
    { 
      WIFIBATTLEMATCH_NET_StartEvilCheck( p_wk->p_net, p_my_data->pokeparty, WIFIBATTLEMATCH_NET_EVILCHECK_TYPE_PARTY );
      (*p_seq)++;
      NAGI_Printf( "EvilCheck Start\n" );
    }
    break;

  case SEQ_WAIT_EVIL_CHECK:
    {
      WIFIBATTLEMATCH_NET_EVILCHECK_RET ret;
      ret = WIFIBATTLEMATCH_NET_WaitEvilCheck( p_wk->p_net, &p_wk->evilecheck_data );
      if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_SUCCESS )
      { 
        NAGI_Printf( "EvilCheck Success\n" );
        (*p_seq)  = SEQ_END_CHECK;
      }
      else if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_ERROR )
      { 
        NAGI_Printf( "EvilCheck Error\n" );
        p_wk->subseq_ret  = WBM_RND_SUBSEQ_EVILCHECK_RET_NET_ERR;
        *p_seq  = SEQ_END;
      }
    }
    break;

  case SEQ_END_CHECK:
    //�f�[�^���i�[
    OS_TPrintf( "�|�P��������[�s���ԍ�%d]status=%d\n", p_wk->evilecheck_data.poke_result[0], p_wk->evilecheck_data.status_code );
    if( p_wk->evilecheck_data.status_code == 0 )
    { 
      //������ۑ�
      GFL_STD_MemCopy( p_wk->evilecheck_data.sign, p_my_data->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );

      NAGI_Printf( "EvilCheck �����I�I\n" );
      p_wk->subseq_ret  = WBM_RND_SUBSEQ_EVILCHECK_RET_SUCCESS;
      *p_seq  = SEQ_END;
    }
    else
    { 
      NAGI_Printf( "EvilCheck �s���I�I\n" );
      p_wk->subseq_ret  = WBM_RND_SUBSEQ_EVILCHECK_RET_DARTY;
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
 *    �֗��֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �����̏���\��
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_Create( WIFIBATTLEMATCH_RND_WORK *p_wk, WIFIBATTLEMATCH_CORE_RETMODE mode )
{ 
  if( p_wk->p_playerinfo == NULL )
  {
    MYSTATUS    *p_my;
    GFL_CLUNIT	*p_unit;
    BOOL is_rate;

    //�����̃f�[�^��\��
    PLAYERINFO_RANDOMMATCH_DATA info_setup;

    p_my  = GAMEDATA_GetMyStatus( p_wk->p_param->p_param->p_game_data); 
    p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );

    GFL_STD_MemClear( &info_setup, sizeof(PLAYERINFO_RANDOMMATCH_DATA) );


    if( mode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      info_setup.btl_rule = p_wk->p_param->p_param->btl_rule;
      is_rate = TRUE;

      info_setup.rate     = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_RATE );
      info_setup.win_cnt  = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      info_setup.lose_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );
      info_setup.btl_cnt  = info_setup.win_cnt + info_setup.lose_cnt;

      if( info_setup.rate == 0 && info_setup.btl_cnt == 0 )
      { 
        info_setup.rate = WIFIBATTLEMATCH_GDB_DEFAULT_RATEING;
      }
    }
    else if( mode == WIFIBATTLEMATCH_CORE_RETMODE_FREE )
    { 
      is_rate = FALSE;
      info_setup.btl_rule = p_wk->p_param->p_param->btl_rule;
      info_setup.rate     = 0;
      info_setup.win_cnt  = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      info_setup.lose_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );
      info_setup.btl_cnt  = info_setup.win_cnt + info_setup.lose_cnt;
    }
    info_setup.trainerID  = MyStatus_GetTrainerView( p_my );
    p_wk->p_playerinfo	= PLAYERINFO_RND_Init( &info_setup, is_rate, p_my, p_unit, p_wk->p_res, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, HEAPID_WIFIBATTLEMATCH_CORE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̏���j��
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_Delete( WIFIBATTLEMATCH_RND_WORK *p_wk )
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
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ���[�N
 *
 *	@return TRUE�Ŋ���  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL Util_PlayerInfo_Move( WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  return PLAYERINFO_MoveMain( p_wk->p_playerinfo );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h���쐬  ���łɃX���C�h�C�����Ă����
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ���[�N
 *	@param	mode                            ���[�h
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_CreateStay( WIFIBATTLEMATCH_RND_WORK *p_wk, WIFIBATTLEMATCH_CORE_RETMODE mode )
{ 
  Util_PlayerInfo_Create( p_wk, mode );

  while( !Util_PlayerInfo_Move( p_wk ) )
  { 

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�҂̏���\��
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_MatchInfo_Create( WIFIBATTLEMATCH_RND_WORK *p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_enemy_data )
{ 
  if( p_wk->p_matchinfo == NULL )
  { 
    GFL_CLUNIT  *p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    WIFIBATTLEMATCH_TYPE type;
    BOOL is_rate;

    if( p_wk->p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      is_rate = TRUE;
      type  = WIFIBATTLEMATCH_TYPE_RNDRATE;
    }
    else if( p_wk->p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_FREE )
    { 
      is_rate = FALSE;
      type  = WIFIBATTLEMATCH_TYPE_RNDFREE;
    }

    p_wk->p_matchinfo		= MATCHINFO_Init( cp_enemy_data, p_unit, p_wk->p_res, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, type, is_rate, REGULATION_CARD_BGM_TRAINER, HEAPID_WIFIBATTLEMATCH_CORE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�҂̏���j��
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_MatchInfo_Delete( WIFIBATTLEMATCH_RND_WORK *p_wk )
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
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ���[�N
 *
 *	@return TRUE�Ŋ���  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL Util_MatchInfo_Main( WIFIBATTLEMATCH_RND_WORK *p_wk )
{
  return MATCHINFO_MoveMain( p_wk->p_matchinfo );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u  ���[�g���[�h�p
 *
 *	@param	RNDMATCH_DATA *p_save �Z�[�u
 *	@param	mode                  ���[��
 *	@param	WIFIBATTLEMATCH_RND_WORK *cp_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_SaveRateScore( RNDMATCH_DATA *p_save, WIFIBATTLEMATCH_BTLRULE btl_rule, const WIFIBATTLEMATCH_RND_WORK *cp_wk )
{ 
  u32 rate, win_cnt, lose_cnt, btl_cnt;

  switch( btl_rule )
  { 
  case WIFIBATTLEMATCH_BTLRULE_SINGLE:  
    rate     = cp_wk->p_param->p_rnd_sake_data->single_rate;
    win_cnt  = cp_wk->p_param->p_rnd_sake_data->single_win;
    lose_cnt = cp_wk->p_param->p_rnd_sake_data->single_lose;
    btl_cnt  = cp_wk->p_param->p_rnd_sake_data->single_win + cp_wk->p_param->p_rnd_sake_data->single_lose;
    break;

  case WIFIBATTLEMATCH_BTLRULE_DOUBLE:
    rate     = cp_wk->p_param->p_rnd_sake_data->double_rate;
    win_cnt  = cp_wk->p_param->p_rnd_sake_data->double_win;
    lose_cnt = cp_wk->p_param->p_rnd_sake_data->double_lose;
    btl_cnt  = cp_wk->p_param->p_rnd_sake_data->double_win + cp_wk->p_param->p_rnd_sake_data->double_lose;
    break;

  case WIFIBATTLEMATCH_BTLRULE_TRIPLE:
    rate     = cp_wk->p_param->p_rnd_sake_data->triple_rate;
    win_cnt  = cp_wk->p_param->p_rnd_sake_data->triple_win;
    lose_cnt = cp_wk->p_param->p_rnd_sake_data->triple_lose;
    btl_cnt  = cp_wk->p_param->p_rnd_sake_data->triple_win + cp_wk->p_param->p_rnd_sake_data->triple_lose;
    break;

  case WIFIBATTLEMATCH_BTLRULE_ROTATE:
    rate     = cp_wk->p_param->p_rnd_sake_data->rot_rate;
    win_cnt  = cp_wk->p_param->p_rnd_sake_data->rot_win;
    lose_cnt = cp_wk->p_param->p_rnd_sake_data->rot_lose;
    btl_cnt  = cp_wk->p_param->p_rnd_sake_data->rot_win + cp_wk->p_param->p_rnd_sake_data->rot_lose;
    break;

  case WIFIBATTLEMATCH_BTLRULE_SHOOTER:
    rate     = cp_wk->p_param->p_rnd_sake_data->shooter_rate;
    win_cnt  = cp_wk->p_param->p_rnd_sake_data->shooter_win;
    lose_cnt = cp_wk->p_param->p_rnd_sake_data->shooter_lose;
    btl_cnt  = cp_wk->p_param->p_rnd_sake_data->shooter_win + cp_wk->p_param->p_rnd_sake_data->shooter_lose;
    break;
  } 

  //�Z�[�u�f�[�^
  RNDMATCH_SetParam( p_save, RNDMATCH_TYPE_RATE_SINGLE + btl_rule, RNDMATCH_PARAM_IDX_RATE, rate );
  RNDMATCH_SetParam( p_save, RNDMATCH_TYPE_RATE_SINGLE + btl_rule, RNDMATCH_PARAM_IDX_WIN, win_cnt );
  RNDMATCH_SetParam( p_save, RNDMATCH_TYPE_RATE_SINGLE + btl_rule, RNDMATCH_PARAM_IDX_LOSE, btl_cnt );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u  �t���[���[�h�p
 *
 *	@param	RNDMATCH_DATA *p_save �Z�[�u
 *	@param	mode                  ���[��
 *	@param	WIFIBATTLEMATCH_RND_WORK *cp_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_SaveFreeScore( RNDMATCH_DATA *p_rndmatch, WIFIBATTLEMATCH_BTLRULE btl_rule, BtlResult btl_result )
{ 
  OS_TFPrintf( 3, "�X�R�A�퓬���� %d", btl_result );
  switch( btl_result )
  {
  case BTL_RESULT_WIN:
  case BTL_RESULT_RUN_ENEMY:
    RNDMATCH_AddParam( p_rndmatch, btl_rule, RNDMATCH_PARAM_IDX_WIN );
    break;
  case BTL_RESULT_LOSE:
  case BTL_RESULT_RUN:
    RNDMATCH_AddParam( p_rndmatch, btl_rule, RNDMATCH_PARAM_IDX_LOSE );
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g������
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ���[�N
 *	@param	type                            ���X�g�̎��
 */
//-----------------------------------------------------------------------------
static void Util_List_Create( WIFIBATTLEMATCH_RND_WORK *p_wk, UTIL_LIST_TYPE type )
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
      setup.strID[0]= WIFIMATCH_TEXT_007;
      setup.strID[1]= WIFIMATCH_TEXT_008;
      setup.list_max= 2;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 1;
      break;

    case UTIL_LIST_TYPE_FREERATE:
      setup.strID[0]= WIFIMATCH_TEXT_003;
      setup.strID[1]= WIFIMATCH_TEXT_004;
      setup.strID[2]= WIFIMATCH_TEXT_005;
      setup.list_max= 3;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 2;
      break;
    }

    p_wk->list_type = type;
    p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�j��
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_List_Delete( WIFIBATTLEMATCH_RND_WORK *p_wk )
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
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ���[�N
 *
 *	@return �I����������
 */
//-----------------------------------------------------------------------------
static u32 Util_List_Main( WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  if( p_wk->p_list )
  { 
    return  WBM_LIST_Main( p_wk->p_list );
  }
  else
  { 
    NAGI_Printf( "List not exist !!!\n" );
    return WBM_LIST_SELECT_NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���O�p�f�[�^�ݒ�
 *
 *	@param	WIFIBATTLEMATCH_MATCH_KEY_DATA *p_data  �f�[�^
 *	@param	WIFIBATTLEMATCH_RND_WORK *cp_wk         ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_Matchkey_SetData( WIFIBATTLEMATCH_MATCH_KEY_DATA *p_data, const WIFIBATTLEMATCH_RND_WORK *cp_wk )
{ 
  GFL_STD_MemClear( p_data, sizeof(WIFIBATTLEMATCH_MATCH_KEY_DATA) );

  p_data->disconnect = cp_wk->p_param->p_rnd_sake_data->disconnect;
  p_data->cup_no = 0;

  //�S������
  p_data->btlcnt  =  cp_wk->p_param->p_rnd_sake_data->single_win + cp_wk->p_param->p_rnd_sake_data->single_lose //�V���O��
    + cp_wk->p_param->p_rnd_sake_data->double_win + cp_wk->p_param->p_rnd_sake_data->double_lose   //�_�u��
    + cp_wk->p_param->p_rnd_sake_data->triple_win + cp_wk->p_param->p_rnd_sake_data->triple_lose   //�g���v��
    + cp_wk->p_param->p_rnd_sake_data->rot_win + cp_wk->p_param->p_rnd_sake_data->rot_lose   //���[�e�[�V����
    + cp_wk->p_param->p_rnd_sake_data->shooter_win + cp_wk->p_param->p_rnd_sake_data->shooter_lose;  //�V���[�^�[

  switch( cp_wk->p_param->p_param->btl_rule )
  { 
  case WIFIBATTLEMATCH_BTLRULE_SINGLE:  
    p_data->rate    = cp_wk->p_param->p_rnd_sake_data->single_rate;
    break;

  case WIFIBATTLEMATCH_BTLRULE_DOUBLE:
    p_data->rate     = cp_wk->p_param->p_rnd_sake_data->double_rate;
    break;

  case WIFIBATTLEMATCH_BTLRULE_TRIPLE:
    p_data->rate     = cp_wk->p_param->p_rnd_sake_data->triple_rate;
    break;

  case WIFIBATTLEMATCH_BTLRULE_ROTATE:
    p_data->rate     = cp_wk->p_param->p_rnd_sake_data->rot_rate;
    break;

  case WIFIBATTLEMATCH_BTLRULE_SHOOTER:
    p_data->rate     = cp_wk->p_param->p_rnd_sake_data->shooter_rate;
    break;
  } 

}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̃f�[�^���i�[ 
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data    �f�[�^
 *	@param	WIFIBATTLEMATCH_RND_WORK *cp_wk         ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_InitMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  GAMEDATA  *p_game_data  = p_wk->p_param->p_param->p_game_data;
  SAVE_CONTROL_WORK *p_sv            = GAMEDATA_GetSaveControlWork(p_game_data);

  p_my_data->btl_server_version  = BTL_NET_SERVER_VERSION;
  {
    MYSTATUS  *p_my;
    p_my  = GAMEDATA_GetMyStatus(p_game_data);
    GFL_STD_MemCopy( p_my, p_my_data->mystatus, MyStatus_GetWorkSize() );
  }
  { 
    const MYPMS_DATA *cp_mypms;
    cp_mypms        = SaveData_GetMyPmsDataConst( p_sv );
    MYPMS_GetPms( cp_mypms, MYPMS_PMS_TYPE_INTRODUCTION, &p_my_data->pms );
  }
  { 
    if( p_wk->p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      p_my_data->win_cnt  = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      p_my_data->lose_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );
    }
    else
    { 
      p_my_data->win_cnt  = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_FREE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      p_my_data->lose_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_FREE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );
    }
  }
  { 
    //PID
    WIFI_LIST *p_wlist = GAMEDATA_GetWiFiList( p_game_data );
    p_my_data->profileID  = WifiList_GetMyGSID( p_wlist );
  }
#if 0 //sys��procInit�ōs���Ă���
  {
    POKEPARTY *p_temoti;
    switch( p_wk->p_param->p_param->poke )
    {
    case WIFIBATTLEMATCH_POKE_TEMOTI:
      p_temoti = GAMEDATA_GetMyPokemon(p_game_data);
      GFL_STD_MemCopy( p_temoti, p_my_data->pokeparty, PokeParty_GetWorkSize() );
      break;

    case WIFIBATTLEMATCH_POKE_BTLBOX:
      { 
        BATTLE_BOX_SAVE *p_btlbox_sv = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
        p_temoti  = BATTLE_BOX_SAVE_MakePokeParty( p_btlbox_sv, GFL_HEAP_LOWID(HEAPID_WIFIBATTLEMATCH_SYS) );
        GFL_STD_MemCopy( p_temoti, p_my_data->pokeparty, PokeParty_GetWorkSize() );
        GFL_HEAP_FreeMemory( p_temoti );
      }
      break;
    }
  }
#endif
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃f�[�^�X�V
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data  ���[�N
 *	@param	*p_wk   ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_RenewalMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  { 
    if( p_wk->p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      p_my_data->win_cnt  = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      p_my_data->lose_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );

      p_my_data->rate = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_RATE );
    }
    else
    { 
      p_my_data->win_cnt  = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_FREE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      p_my_data->lose_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_FREE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �T�u�V�[�P���X�J�n
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ���[�N
 *	@param	seq_function                      �T�u�V�[�P���X
 */
//-----------------------------------------------------------------------------
static void Util_SubSeq_Start( WIFIBATTLEMATCH_RND_WORK *p_wk, WBM_SEQ_FUNCTION seq_function )
{ 
  WBM_SEQ_SetNext( p_wk->p_subseq, seq_function );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �T�u�V�[�P���X��
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ���[�N
 *	@return WBM_RND_SUBSEQ_RET_NONE���쒆  ����ȊO�̂Ƃ��̓T�u�V�[�P���X���I�����߂�l��Ԃ��Ă���
 *
 *	@return �߂�l
 */
//-----------------------------------------------------------------------------
static WBM_RND_SUBSEQ_RET Util_SubSeq_Main( WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  WBM_SEQ_Main( p_wk->p_subseq );
  if( WBM_SEQ_IsEnd( p_wk->p_subseq ) )
  {
    return p_wk->subseq_ret;
  }
  else
  { 
    return WBM_RND_SUBSEQ_RET_NONE;
  }
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
static BOOL Util_VerifyPokeData( WIFIBATTLEMATCH_ENEMYDATA *p_data, HEAPID heapID )
{ 
  int i;
  void *p_buff;
  POKEPARTY *p_party  = (POKEPARTY *)p_data->pokeparty;
  const POKEMON_PARAM *cp_pp;
  BOOL ret;

  p_buff  = NHTTP_RAP_EVILCHECK_CreateVerifyPokeBuffer( POKETOOL_GetWorkSize(), PokeParty_GetPokeCount(p_party) , GFL_HEAP_LOWID(heapID) );

  for( i = 0; i < PokeParty_GetPokeCount( p_party ); i++ )
  { 
    cp_pp  = PokeParty_GetMemberPointer( p_party, i);
    NHTTP_RAP_EVILCHECK_AddPokeVerifyPokeBuffer( p_buff, cp_pp, POKETOOL_GetWorkSize(), i );
    OS_TPrintf( "monsno %d\n", PP_Get( cp_pp, ID_PARA_monsno, NULL) );
    OS_TPrintf( "rnd %d\n", PP_Get( cp_pp, ID_PARA_personal_rnd, NULL) );
    OS_TPrintf( "crc %d\n", GFL_STD_CrcCalc( cp_pp, POKETOOL_GetWorkSize() ) );
  }

  ret = NHTTP_RAP_EVILCHECK_VerifySign( p_buff, POKETOOL_GetWorkSize(), PokeParty_GetPokeCount(p_party) , p_data->sign, GFL_HEAP_LOWID(heapID) );

  NHTTP_RAP_EVILCHECK_DeleteVerifyPokeBuffer( p_buff );

  NAGI_Printf( "�s���`�F�b�N�ʉ߁H=[%d]\n", ret );
  NAGI_Printf( "�p�[�e�B=[%d]\n", PokeParty_GetPokeCount( p_party ) );
  
  { 
    int i;
    for( i = 0; i < NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN; i++ )
    { 
      OS_TPrintf( "%d ", p_data->sign[i] );
      if(i % 0x10 == 0xF )
      { 
        OS_TPrintf( "\n" );
      }
    }
  }


  return ret;

}
//----------------------------------------------------------------------------
/**
 *	@brief  �L�����Z������
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ���[�N
 *	@param  is_cancel TRUE�Ȃ�΃L�����Z���������Ȃ��Ă悢�@FALSE�Ȃ�΃_��
 *
 *	@retval �L�����Z�����
 */
//-----------------------------------------------------------------------------
static UTIL_CANCEL_STATE Util_Cancel_Seq( WIFIBATTLEMATCH_RND_WORK *p_wk, BOOL is_cancel )
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
    if( is_cancel && (GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL) )
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
    if( WIFIBATTLEMATCH_NET_SendMatchCancel( p_wk->p_net ) )
    {
      p_wk->cancel_seq++;
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

//=============================================================================
/**
 *    DEBUG
 */
//=============================================================================
#ifdef DEBUGWIN_USE
static void DEBUGWIN_VisiblePlayerInfo( void* userWork , DEBUGWIN_ITEM* item );
static void DEBUGWIN_VisibleMatchInfo( void* userWork , DEBUGWIN_ITEM* item );
static void DEBUGWIN_ChangePlayerInfo( void* userWork , DEBUGWIN_ITEM* item );
static void DEBUGWIN_ChangeMatchInfo( void* userWork , DEBUGWIN_ITEM* item );
static void DEBUGWIN_ChangeWaitIcon( void* userWork , DEBUGWIN_ITEM* item );


static void DEBUGWIN_CARD_Init( WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  DEBUGWIN_AddGroupToTop( DEBUGWIN_GROUP_CARD, "�J�[�h", HEAPID_WIFIBATTLEMATCH_CORE );
  DEBUGWIN_AddItemToGroup( "���Ԃ�J�[�h", 
                              DEBUGWIN_VisiblePlayerInfo,
                              p_wk, 
                              DEBUGWIN_GROUP_CARD, 
                              HEAPID_WIFIBATTLEMATCH_CORE );

  DEBUGWIN_AddItemToGroup( "�����ăJ�[�h", 
                              DEBUGWIN_VisibleMatchInfo, 
                              p_wk, 
                              DEBUGWIN_GROUP_CARD, 
                              HEAPID_WIFIBATTLEMATCH_CORE );

  DEBUGWIN_AddItemToGroup( "���Ԃ񃉃��N", 
                              DEBUGWIN_ChangePlayerInfo, 
                              p_wk, 
                              DEBUGWIN_GROUP_CARD, 
                              HEAPID_WIFIBATTLEMATCH_CORE );

  DEBUGWIN_AddItemToGroup( "�����ă����N", 
                              DEBUGWIN_ChangeMatchInfo, 
                              p_wk, 
                              DEBUGWIN_GROUP_CARD, 
                              HEAPID_WIFIBATTLEMATCH_CORE );

  DEBUGWIN_AddItemToGroup( "���邮��[ON]", 
                              DEBUGWIN_ChangeWaitIcon, 
                              p_wk, 
                              DEBUGWIN_GROUP_CARD, 
                              HEAPID_WIFIBATTLEMATCH_CORE );
}

static void DEBUGWIN_CARD_Exit( WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  DEBUGWIN_RemoveGroup( DEBUGWIN_GROUP_CARD );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h�\��
 *
 *	@param	void* userWork  ���[�N
 *	@param	item            �A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGWIN_VisiblePlayerInfo( void* userWork , DEBUGWIN_ITEM* item )
{ 
  static const char *scp_name_tbl[] =
  { 
    "���[�g",
    "�t���[",
  };

  WIFIBATTLEMATCH_RND_WORK  *p_wk = userWork;

  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
  { 

    p_wk->playerinfo_mode++;
    p_wk->playerinfo_mode %= NELEMS( scp_name_tbl );

    Util_PlayerInfo_Delete( p_wk );
    Util_PlayerInfo_Create( p_wk, p_wk->playerinfo_mode );

    while( !Util_PlayerInfo_Move( p_wk ) )
    { 
      GFL_BG_VBlankFunc();
      GFL_CLACT_SYS_VBlankFunc();
      PLAYERINFO_PrintMain( p_wk->p_playerinfo, p_wk->p_que );
    }

    DEBUGWIN_ITEM_SetNameV( item , "���Ԃ�J�[�h[%s]", scp_name_tbl[ p_wk->playerinfo_mode ] );
    DEBUGWIN_RefreshScreen();
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�҂̃J�[�h�\��
 *
 *	@param	void* userWork  ���[�N
 *	@param	item            �A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGWIN_VisibleMatchInfo( void* userWork , DEBUGWIN_ITEM* item )
{ 

  WIFIBATTLEMATCH_RND_WORK  *p_wk = userWork;

  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
  { 
    WIFIBATTLEMATCH_DATA_ModifiEnemyData( p_wk->p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );

    Util_MatchInfo_Delete( p_wk );
    Util_MatchInfo_Create( p_wk, p_wk->p_param->p_enemy_data );

    while( !Util_MatchInfo_Main( p_wk ) )
    { 
      GFL_BG_VBlankFunc();
      GFL_CLACT_SYS_VBlankFunc();
      MATCHINFO_PrintMain( p_wk->p_matchinfo, p_wk->p_que );
    }

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h�؂�ւ�
 *
 *	@param	void* userWork  ���[�N
 *	@param	item            �A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGWIN_ChangePlayerInfo( void* userWork , DEBUGWIN_ITEM* item )
{ 
  static const char *scp_name_tbl[] =
  { 
    "�u���b�N",
    "�S�[���h",
    "�V���o�[",
    "�u�����Y",
    "�J�b�p�[",
    "�m�[�}��",
  };

  WIFIBATTLEMATCH_RND_WORK  *p_wk = userWork;
  u32 win;

  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
  { 

    p_wk->playerinfo_rank++;
    p_wk->playerinfo_rank %= NELEMS( scp_name_tbl );

    switch( p_wk->playerinfo_rank )
    { 
    case 0: //�u���b�N
      win = WBM_CARD_WIN_BLACK;
      break;
    case 1:  //�S�[���h
      win = WBM_CARD_WIN_GOLD;
      break;
    case 2: //�V���o�[
      win = WBM_CARD_WIN_SILVER;
      break;
    case 3: //�u�����Y
      win = WBM_CARD_WIN_BRONZE;
      break;
    case 4: //�J�b�p�[
      win = WBM_CARD_WIN_COPPER;
      break;
    case 5: //�m�[�}��
      win = WBM_CARD_WIN_NORMAL;
      break;
    }
    p_wk->p_param->p_rnd_sake_data->single_win  = win;
    p_wk->p_param->p_rnd_sake_data->double_win  = win;
    p_wk->p_param->p_rnd_sake_data->triple_win  = win;
    p_wk->p_param->p_rnd_sake_data->rot_win  = win;
    p_wk->p_param->p_rnd_sake_data->shooter_win  = win;
    RNDMATCH_SetParam( p_wk->p_param->p_rndmatch, p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN, win );

    Util_PlayerInfo_Delete( p_wk );
    Util_PlayerInfo_Create( p_wk, p_wk->playerinfo_mode );

    while( !Util_PlayerInfo_Move( p_wk ) )
    { 
      GFL_BG_VBlankFunc();
      GFL_CLACT_SYS_VBlankFunc();
      PLAYERINFO_PrintMain( p_wk->p_playerinfo, p_wk->p_que );
    }

    DEBUGWIN_ITEM_SetNameV( item , "���Ԃ񃉃��N[%s]", scp_name_tbl[ p_wk->playerinfo_rank ] );
    DEBUGWIN_RefreshScreen();
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�҂̃J�[�h�؂�ւ�
 *
 *	@param	void* userWork  ���[�N
 *	@param	item            �A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGWIN_ChangeMatchInfo( void* userWork , DEBUGWIN_ITEM* item )
{ 
  static const char *scp_name_tbl[] =
  { 
    "�u���b�N",
    "�S�[���h",
    "�V���o�[",
    "�u�����Y",
    "�J�b�p�[",
    "�m�[�}��",
  };

  WIFIBATTLEMATCH_RND_WORK  *p_wk = userWork;
  u32 win;

  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
  { 
    p_wk->matchinfo_rank++;
    p_wk->matchinfo_rank %= NELEMS( scp_name_tbl );

    switch( p_wk->matchinfo_rank )
    { 
    case 0: //�u���b�N
      win = WBM_CARD_WIN_BLACK;
      break;
    case 1:  //�S�[���h
      win = WBM_CARD_WIN_GOLD;
      break;
    case 2: //�V���o�[
      win = WBM_CARD_WIN_SILVER;
      break;
    case 3: //�u�����Y
      win = WBM_CARD_WIN_BRONZE;
      break;
    case 4: //�J�b�p�[
      win = WBM_CARD_WIN_COPPER;
      break;
    case 5: //�m�[�}��
      win = WBM_CARD_WIN_NORMAL;
      break;
    }
    p_wk->p_param->p_enemy_data->win_cnt = win;

    WIFIBATTLEMATCH_DATA_ModifiEnemyData( p_wk->p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );

    Util_MatchInfo_Delete( p_wk );
    Util_MatchInfo_Create( p_wk, p_wk->p_param->p_enemy_data );

    while( !Util_MatchInfo_Main( p_wk ) )
    { 
      GFL_BG_VBlankFunc();
      GFL_CLACT_SYS_VBlankFunc();
      MATCHINFO_PrintMain( p_wk->p_matchinfo, p_wk->p_que );
    }

    DEBUGWIN_ITEM_SetNameV( item , "�����ă����N[%s]", scp_name_tbl[ p_wk->matchinfo_rank ] );
    DEBUGWIN_RefreshScreen();
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �A�C�R���\��
 *
 *	@param	void* userWork  ���[�N
 *	@param	item            �A�C�e��
 */
//-----------------------------------------------------------------------------
static void DEBUGWIN_ChangeWaitIcon( void* userWork , DEBUGWIN_ITEM* item )
{
  static const char *scp_name_tbl[] =
  { 
    "OFF",
    "ON",
  };

  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
  { 
    WIFIBATTLEMATCH_RND_WORK  *p_wk = userWork;

    p_wk->waiticon_draw++;
    p_wk->waiticon_draw %= NELEMS( scp_name_tbl );

    switch( p_wk->waiticon_draw )
    { 
    case 0:
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      break;
    case 1:
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, TRUE );
      break;
    }

    DEBUGWIN_ITEM_SetNameV( item , "���邮��[%s]", scp_name_tbl[ p_wk->waiticon_draw ] );
    DEBUGWIN_RefreshScreen();
  }
}




#endif //DEBUGWIN_USE
