//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_sys.c
 *	@brief  �e�v���Z�X�̂Ȃ�
 *	@author	Toru=Nagihashi
 *	@data		2009.11.29
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
#include "net/network_define.h"
#include "poke_tool/poke_regulation.h"
#include "sound/pm_sndsys.h"
#include "system/net_err.h"
#include "net/dreamworld_netdata.h"

//�e�v���Z�X
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"
#include "title/title.h"
#include "wifibattlematch_battle.h"
#include "net_app/btl_rec_sel.h"
#include "wifibattlematch_utilproc.h"
#include "wifibattlematch_subproc.h"
#include "battle_championship_core.h"

//�Z�[�u�f�[�^
#include "savedata/battle_box_save.h"
#include "savedata/wifihistory.h"
#include "savedata/battlematch_savedata.h"
#include "savedata/my_pms_data.h"

//�����̃��W���[��
#include "wifibattlematch_core.h"
#include "wifibattlematch_util.h"
#include "wifibattlematch_data.h"
#include "wifibattlematch_snd.h"

//�O�����J
#include "net_app/wifibattlematch.h"

//�f�o�b�O
#include "debug/debug_nagihashi.h"

//-------------------------------------
///	DEBUG
//=====================================
#ifdef PM_DEBUG
#endif //PM_DEBUG

//-------------------------------------
///	PROC�G�N�X�^�[��
//=====================================
FS_EXTERN_OVERLAY( wifibattlematch_proc );

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//SAKE�T�[�o�[��ɂ�80�o�C�g�����Ƃ��Ă��Ȃ�
SDK_COMPILER_ASSERT( sizeof(WIFIBATTLEMATCH_RECORD_DATA) == 80 );

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���[�N
//=====================================
typedef struct _WBM_SYS_SUBPROC_WORK WBM_SYS_SUBPROC_WORK;


//-------------------------------------
///	�V�X�e�����[�N
//=====================================
typedef struct
{ 
  BOOL                        is_create_gamedata;
  WIFIBATTLEMATCH_PARAM       param;

  //�v���Z�X�Ǘ��V�X�e��
  WBM_SYS_SUBPROC_WORK        *p_subproc;

  //���[�h
  WIFIBATTLEMATCH_TYPE        type;

  //�ȉ�PROC�̂Ȃ��̂��߂ɋL�����Ă�������
  WIFIBATTLEMATCH_CORE_MODE   core_mode;

  //�ȉ��V�X�e���w�ɒu���Ă����f�[�^
  WIFIBATTLEMATCH_ENEMYDATA   *p_player_data;
  WIFIBATTLEMATCH_ENEMYDATA   *p_enemy_data;
  POKEPARTY                   *p_player_btl_party;//�����Ō��߂��p�[�e�B
  POKEPARTY                   *p_enemy_btl_party; //����̌��߂��p�[�e�B
  POKEPARTY                   *p_player_modify_party;//���x���␳���������p�[�e�B
  POKEPARTY                   *p_enemy_modify_party; //
  DWCSvlResult                svl_result;         //WIFI���O�C�����ɓ���T�[�r�X���P�[�^
  BATTLEMATCH_BATTLE_SCORE    btl_score;          //�o�g���̐���
  u32                         server_time;        //�T�[�o�A�N�Z�X����
  WIFIBATTLEMATCH_RECORD_DATA record_data;        //���
  WIFIBATTLEMATCH_RECV_DATA   recv_data;          //�T�[�o�[�����M�����f�[�^
  BOOL                        is_err_return_login;//WIFILOGIN�ɃG���[�Ŗ߂�Ƃ�
  DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA gpf_data;//GPF�T�[�o�[���痎�Ƃ��Ă����I��؃f�[�^
  WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA   sake_data;

} WIFIBATTLEMATCH_SYS;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	�T�u�v���Z�X�p�����̉���A�j���֐�
//=====================================
//���C�����j���[�{LIVE�p�t���[�v���Z�X
static void *BC_CORE_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc, HEAPID heapID, void *p_wk_adrs );
static BOOL BC_CORE_FreeParam(  WBM_SYS_SUBPROC_WORK *p_subproc, void *p_param_adrs, void *p_wk_adrs );
//WIFI�t���[���C���v���Z�X
static void *WBM_CORE_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs );
static BOOL WBM_CORE_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs );
//���X�g�{�X�e�[�^�X
static void *POKELIST_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs );
static BOOL POKELIST_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs );
//�o�g���{�f��
static void *BATTLE_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs );
static BOOL BATTLE_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs );
//WIFI���O�C��
static void *LOGIN_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs );
static BOOL LOGIN_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs );
//���X�g�{�X�e�[�^�X�`�o�g���{�f���ւ̂Ȃ��v���Z�X
static void *WBM_LISTAFTER_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs );
static BOOL WBM_LISTAFTER_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs );
//�^��
static void *WBM_BTLREC_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs );
static BOOL WBM_BTLREC_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs );
//WIFI���O�A�E�g
static void *LOGOUT_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs );
static BOOL LOGOUT_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs );
//�^��Đ�
static void *RECPLAY_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs );
static BOOL RECPLAY_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	�f�[�^�o�b�t�@�쐬
//=====================================
static void DATA_CreateBuffer( WIFIBATTLEMATCH_SYS *p_wk, HEAPID heapID );
static void DATA_DeleteBuffer( WIFIBATTLEMATCH_SYS *p_wk );

//���̑�
static void Util_SetRecordData( WIFIBATTLEMATCH_RECORD_DATA *p_data, const POKEPARTY *cp_my_poke, const POKEPARTY *cp_you_poke, const WIFIBATTLEMATCH_ENEMYDATA *cp_you_data, const REGULATION *cp_reg, u32 cupNO, BtlResult result, WIFIBATTLEMATCH_TYPE type );

//=============================================================================
/**
 *				  �T�u�v���Z�X
 *				    �E�v���Z�X���s��������V�X�e��
 */
//=============================================================================
//-------------------------------------
///	�T�u�v���Z�X�������E����֐��R�[���o�b�N
//=====================================
typedef void *(*WBM_SYS_SUBPROC_ALLOC_FUNCTION)( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs );
typedef BOOL (*WBM_SYS_SUBPROC_FREE_FUNCTION)( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param, void *p_wk_adrs );

//-------------------------------------
///	�T�u�v���Z�X�ݒ�\����
//=====================================
typedef struct 
{
	FSOverlayID							    ov_id;
	const GFL_PROC_DATA			    *cp_procdata;
	WBM_SYS_SUBPROC_ALLOC_FUNCTION	alloc_func;
	WBM_SYS_SUBPROC_FREE_FUNCTION		free_func;
} WBM_SYS_SUBPROC_DATA;

//-------------------------------------
///	�p�u���b�N
//=====================================
static  WBM_SYS_SUBPROC_WORK * WBM_SYS_SUBPROC_Init( const WBM_SYS_SUBPROC_DATA *cp_procdata_tbl, u32 tbl_len, void *p_wk_adrs, HEAPID heapID );
static void WBM_SYS_SUBPROC_Exit( WBM_SYS_SUBPROC_WORK *p_wk );
static BOOL WBM_SYS_SUBPROC_Main( WBM_SYS_SUBPROC_WORK *p_wk );
static void WBM_SYS_SUBPROC_CallProc( WBM_SYS_SUBPROC_WORK *p_wk, u32 procID );
static void WBM_SYS_SUBPROC_End( WBM_SYS_SUBPROC_WORK *p_wk );
static GFL_PROC_MAIN_STATUS WBM_SYS_SUBPROC_GetStatus( const WBM_SYS_SUBPROC_WORK *cp_wk );
static u8 WBM_SYS_SUBPROC_GetPreProcID( const WBM_SYS_SUBPROC_WORK *cp_wk );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	WifiBattleMatch_ProcData =
{	
	WIFIBATTLEMATCH_PROC_Init,
	WIFIBATTLEMATCH_PROC_Main,
	WIFIBATTLEMATCH_PROC_Exit,
};

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//-------------------------------------
///	�T�u�v���Z�X�ړ��f�[�^
//=====================================
enum
{	
	SUBPROCID_MAINMENU,
	SUBPROCID_CORE,
  SUBPROCID_POKELIST,
  SUBPROCID_BATTLE,
  SUBPROCID_LOGIN,
  SUBPROCID_LISTAFTER,
  SUBPROCID_BTLREC,
  SUBPROCID_LOGOUT,
  SUBPROCID_RECPLAY,

	SUBPROCID_MAX
};
static const WBM_SYS_SUBPROC_DATA sc_subproc_data[SUBPROCID_MAX]	=
{	
	//SUBPROCID_MAINMENU,
  { 
		FS_OVERLAY_ID( battle_championship ),
		&BATTLE_CHAMPIONSHIP_CORE_ProcData,
		BC_CORE_AllocParam,
		BC_CORE_FreeParam,
  },
	//SUBPROCID_CORE
	{	
		FS_OVERLAY_ID( wifibattlematch_core ),
		&WifiBattleMatchCore_ProcData,
		WBM_CORE_AllocParam,
		WBM_CORE_FreeParam,
	},
  //SUBPROCID_POKELIST,
  { 
	  FS_OVERLAY_ID( wifibattlematch_proc ),
    &WifiBattleMatch_Sub_ProcData,
    POKELIST_AllocParam,
    POKELIST_FreeParam,
  },
  //SUBPROCID_BATTLE,
  { 
	  NO_OVERLAY_ID,
    &WifiBattleMatch_BattleLink_ProcData,
    BATTLE_AllocParam,
    BATTLE_FreeParam,
  },
  //SUBPROCID_LOGIN
  { 
    FS_OVERLAY_ID( wifi_login ),
    &WiFiLogin_ProcData,
    LOGIN_AllocParam,
    LOGIN_FreeParam,
  },
  //SUBPROCID_LISTAFTER,
  { 
		FS_OVERLAY_ID( wifibattlematch_proc ),
		&WifiBattleMatch_ListAfter_ProcData,
		WBM_LISTAFTER_AllocParam,
		WBM_LISTAFTER_FreeParam,
  },
  //SUBPROCID_BTLREC
  { 
    FS_OVERLAY_ID( btl_rec_sel ),
    &BTL_REC_SEL_ProcData,
    WBM_BTLREC_AllocParam,
    WBM_BTLREC_FreeParam,
  },
  //SUBPROCID_LOGOUT
  { 
    FS_OVERLAY_ID( wifi_login ),
    &WiFiLogout_ProcData,
    LOGOUT_AllocParam,
    LOGOUT_FreeParam,
  },
  //SUBPROCID_RECPLAY,
  { 
    NO_OVERLAY_ID,
    &BtlProcData,
    RECPLAY_AllocParam,
    RECPLAY_FreeParam,
  },
};

//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  WIFIBATTLEMATCH_SYS   *p_wk;
  HEAPID  parentID;

  { 
    WIFIBATTLEMATCH_PARAM *p_param  = p_param_adrs;
    switch( p_param->mode )
    { 
    case WIFIBATTLEMATCH_MODE_RANDOM:
      //�����_���}�b�`�̓|�P�Z��WIFI�J�E���^�[�������A
      //�Q�[���V�X�e�����Ń�������H���Ă���̂ŁAHEAPID_PROC�ɃV�X�e��������
      //parentID  = HEAPID_PROC;
      //PROC�͂��܂��܂ȃC�x���g�����\��������̂�APP����Ⴄ
      parentID  = GFL_HEAPID_APP;
      break;

    case WIFIBATTLEMATCH_MODE_MAINMENU:
      //����ȊO�́A�^�C�g����ʂ��炭�邽��HEAPID_APP������ɂ���̂ŁA
      //HEAPID_APP������炤
      parentID  = GFL_HEAPID_APP;
      break;
    default:
      GF_ASSERT(0);
    }
  }
  
  NAGI_Printf( "work %d + %d *2\n", sizeof(WIFIBATTLEMATCH_SYS), sizeof(WIFIBATTLEMATCH_ENEMYDATA) );

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( parentID, HEAPID_WIFIBATTLEMATCH_SYS, 0x8000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFIBATTLEMATCH_SYS), HEAPID_WIFIBATTLEMATCH_SYS );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_SYS) );
  GFL_STD_MemCopy( p_param_adrs, &p_wk->param, sizeof(WIFIBATTLEMATCH_PARAM) );
  p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_START;

  //�Q�[���f�[�^���������̂Ƃ��͍쐬
  if( p_wk->param.p_game_data == NULL )
  { 
    p_wk->param.p_game_data = GAMEDATA_Create( GFL_HEAPID_APP );
    p_wk->is_create_gamedata  = TRUE;
  }

  //�퓬�p�p�[�e�B�쐬
  p_wk->p_player_btl_party = PokeParty_AllocPartyWork( HEAPID_WIFIBATTLEMATCH_SYS );
  p_wk->p_enemy_btl_party = PokeParty_AllocPartyWork( HEAPID_WIFIBATTLEMATCH_SYS );
  p_wk->p_player_modify_party = PokeParty_AllocPartyWork( HEAPID_WIFIBATTLEMATCH_SYS );
  p_wk->p_enemy_modify_party = PokeParty_AllocPartyWork( HEAPID_WIFIBATTLEMATCH_SYS );

  OS_Printf( "�����_���}�b�`���� �g�p�|�P%d ���[��%d\n", p_wk->param.poke, p_wk->param.btl_rule );

  //�f�[�^�o�b�t�@�쐬
  BattleRec_Init( HEAPID_WIFIBATTLEMATCH_SYS );
  DATA_CreateBuffer( p_wk, HEAPID_WIFIBATTLEMATCH_SYS );

  //���W���[���쐬
	p_wk->p_subproc = WBM_SYS_SUBPROC_Init( sc_subproc_data, SUBPROCID_MAX, p_wk, HEAPID_WIFIBATTLEMATCH_SYS );

  //�J�n���[�h
  switch( p_wk->param.mode )
  { 
  case WIFIBATTLEMATCH_MODE_MAINMENU:  //�^�C�g������i�ށA���C�����j���[
    WBM_SYS_SUBPROC_CallProc( p_wk->p_subproc, SUBPROCID_MAINMENU );
    break;
  case WIFIBATTLEMATCH_MODE_RANDOM:    //�|�P�Z����WIFI�J�E���^�[���炷���ށA�����_���ΐ�
    WBM_SYS_SUBPROC_CallProc( p_wk->p_subproc, SUBPROCID_LOGIN );
    break;
  default:
    GF_ASSERT( 0 );
  }

  //�����̃f�[�^������
  {
    POKEPARTY *p_temoti;
    WIFIBATTLEMATCH_ENEMYDATA *p_my_data  = p_wk->p_player_data;
    switch( p_wk->param.poke )
    {
    case WIFIBATTLEMATCH_POKE_TEMOTI:
      p_temoti = GAMEDATA_GetMyPokemon(p_wk->param.p_game_data);
      GFL_STD_MemCopy( p_temoti, p_my_data->pokeparty, PokeParty_GetWorkSize() );
      break;

    case WIFIBATTLEMATCH_POKE_BTLBOX:
      { 
        SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork(p_wk->param.p_game_data);
        BATTLE_BOX_SAVE *p_btlbox_sv = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
        p_temoti  = BATTLE_BOX_SAVE_MakePokeParty( p_btlbox_sv, GFL_HEAP_LOWID(HEAPID_WIFIBATTLEMATCH_SYS) );
        GFL_STD_MemCopy( p_temoti, p_my_data->pokeparty, PokeParty_GetWorkSize() );
        GFL_HEAP_FreeMemory( p_temoti );
      }
      break;
    }
  }

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	���C���v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  WIFIBATTLEMATCH_SYS   *p_wk     = p_wk_adrs;

	//���W���[���j��
	WBM_SYS_SUBPROC_Exit( p_wk->p_subproc );

  //�f�[�^�o�b�t�@�j��
  DATA_DeleteBuffer( p_wk );
  BattleRec_Exit();

  //�p�[�e�B�̔j��
  GFL_HEAP_FreeMemory( p_wk->p_enemy_modify_party );
  GFL_HEAP_FreeMemory( p_wk->p_player_modify_party );
  GFL_HEAP_FreeMemory( p_wk->p_enemy_btl_party );
  GFL_HEAP_FreeMemory( p_wk->p_player_btl_party );

  //�Q�[���f�[�^�������ō쐬���Ă�����j��
  if( p_wk->is_create_gamedata )
  { 
    GAMEDATA_Delete( p_wk->param.p_game_data );
  }

  //�����_���}�b�`�ȊO�̓^�C�g���֖߂�
  if( p_wk->param.mode != WIFIBATTLEMATCH_MODE_RANDOM )
  { 
    //GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);
    OS_ResetSystem(0);
  }

  //���������j���t���O��ݒ肳��Ă�����j��
  if( p_wk->param.is_auto_release )
  { 
    GFL_HEAP_FreeMemory( p_param_adrs );
  }

  //�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SYS );

  PMSND_StopBGM();

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
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
	enum
	{	
		WBM_SYS_SEQ_INIT,
		WBM_SYS_SEQ_FADEIN,
		WBM_SYS_SEQ_FADEIN_WAIT,
		WBM_SYS_SEQ_MAIN,
		WBM_SYS_SEQ_FADEOUT,
		WBM_SYS_SEQ_FADEOUT_WAIT,
		WBM_SYS_SEQ_EXIT,
	};

  WIFIBATTLEMATCH_SYS   *p_wk     = p_wk_adrs;

  //DEBUG_HEAP_PrintRestUse( GFL_HEAPID_APP );

  //�V�[�P���X
	switch( *p_seq )
	{	
	case WBM_SYS_SEQ_INIT:
		*p_seq	= WBM_SYS_SEQ_MAIN;
		break;

	case WBM_SYS_SEQ_FADEIN:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= WBM_SYS_SEQ_FADEIN_WAIT;
		break;

	case WBM_SYS_SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= WBM_SYS_SEQ_MAIN;
		}
		break;

	case WBM_SYS_SEQ_MAIN:
		{
      //�T�u�v���b�N�̃��[�v
			if( WBM_SYS_SUBPROC_Main( p_wk->p_subproc ) )
			{	
				*p_seq	= WBM_SYS_SEQ_EXIT;
			}

      //SAKE�T�[�o�[�ւ̃A�N�Z�X���Ԃ��J�E���_�E��
      if( p_wk->server_time > 0 )
      { 
        p_wk->server_time--;
      }

      //���̃��[�J��PROC����PROC�J�ڂ��邽�߁A�G���[�V�X�e���������œ�����
      //->gamesystem_main���LOCAL_PROC�œ����̂ł��ׂĂœ������K�v������
      //if( p_wk->param.mode == WIFIBATTLEMATCH_MODE_MAINMENU )
      { 
        const GFL_PROC_MAIN_STATUS  status  = WBM_SYS_SUBPROC_GetStatus( p_wk->p_subproc );
        if( status == GFL_PROC_MAIN_CHANGE || status == GFL_PROC_MAIN_NULL )
        { 
          NetErr_DispCall(FALSE);
        }
      }
		}
		break;

	case WBM_SYS_SEQ_FADEOUT:

    if( p_wk->param.mode != WIFIBATTLEMATCH_MODE_RANDOM )
    { 
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, 0 );
    }
    else
    { 
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    }
		*p_seq	= WBM_SYS_SEQ_FADEOUT_WAIT;
		break;

	case WBM_SYS_SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= WBM_SYS_SEQ_EXIT;
		}
		break;

	case WBM_SYS_SEQ_EXIT:
		return GFL_PROC_RES_FINISH;
	}
	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *		�T�u�v���Z�X�p�쐬�j��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���C�����j���[�{LIVE�p�t���[�R�A�̈���	�쐬
 *
 *	@param	HEAPID heapID			�q�[�vID
 *	@param	*p_wk_adrs				���[�N
 *
 *	@return	����
 */
//-----------------------------------------------------------------------------
static void *BC_CORE_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs )
{	
  BATTLE_CHAMPIONSHIP_CORE_PARAM  *p_param;
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;

  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(BATTLE_CHAMPIONSHIP_CORE_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(BATTLE_CHAMPIONSHIP_CORE_PARAM) );
  p_param->p_gamedata     = p_wk->param.p_game_data;
  p_param->p_player_data  = p_wk->p_player_data;
  p_param->p_enemy_data   = p_wk->p_enemy_data;
  p_param->p_btl_score    = &p_wk->btl_score;

  switch( WBM_SYS_SUBPROC_GetPreProcID( p_subproc ) )
  { 
  case SUBPROCID_LOGOUT:
    //���O�A�E�g���璅�Ă�����AWIFI���i�����_���}�b�`�͂����ւ��Ȃ��̂Łj
    p_param->mode       = BATTLE_CHAMPIONSHIP_CORE_MODE_WIFI_MENU;
    break;

  case SUBPROCID_RECPLAY:
    //�Đ����璅�Ă�����ALIVE���C�����j���[
    p_param->mode       = BATTLE_CHAMPIONSHIP_CORE_MODE_LIVE_FLOW_MENU;
    break;

  case SUBPROCID_BATTLE:
    //�o�g�����炫�Ă�����ALIVE���o�g���㏈��
    p_param->mode       = BATTLE_CHAMPIONSHIP_CORE_MODE_LIVE_FLOW_BTLEND;
    break;

  case SUBPROCID_BTLREC:
    //�^�悩�炫�Ă�����ALIVE���^��㏈��
    p_param->mode       = BATTLE_CHAMPIONSHIP_CORE_MODE_LIVE_FLOW_RECEND;
    break;

  case 0:
    //�������l0�Ȃ�Έ�ԍŏ��ɂ����ւ��������C�����j���[
    p_param->mode       = BATTLE_CHAMPIONSHIP_CORE_MODE_MAIN_MEMU;
    break;

  default:
    if( p_wk->btl_score.is_error )
    { 
      //LIVE���p�G���[
      p_param->mode       = BATTLE_CHAMPIONSHIP_CORE_MODE_LIVE_FLOW_MENU;
    }
    else
    { 
      GF_ASSERT(0);
    }
    break;
  }

  if( PMSND_GetBGMsoundNo() != WBM_SND_SEQ_MAIN )
  { 
    PMSND_PlayBGM( WBM_SND_SEQ_MAIN );
  }
		
	return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���C�����j���[�{LIVE�p�t���[�R�A�̈���	�j��
 *
 *	@param	void *p_param_adrs				����
 *	@param	*p_wk_adrs								���[�N
 */
//-----------------------------------------------------------------------------
static BOOL BC_CORE_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs )
{	
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  BATTLE_CHAMPIONSHIP_CORE_PARAM  *p_param  = p_param_adrs;

  switch( p_param->ret )
  { 
  case BATTLE_CHAMPIONSHIP_CORE_RET_TITLE:   //�^�C�g���֍s��
    WBM_SYS_SUBPROC_End( p_subproc );
    break;
  case BATTLE_CHAMPIONSHIP_CORE_RET_WIFICUP: //WIFI���֍s��
    p_wk->type  = WIFIBATTLEMATCH_TYPE_WIFICUP;
    WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_LOGIN );
    break;
  case BATTLE_CHAMPIONSHIP_CORE_RET_LIVEBTL: //LIVE�p�o�g���֍s��
    p_wk->type  = WIFIBATTLEMATCH_TYPE_LIVECUP;
    WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_POKELIST );
    break;
  case BATTLE_CHAMPIONSHIP_CORE_RET_LIVEREC: //LIVE�p�^��֍s��
    p_wk->type  = WIFIBATTLEMATCH_TYPE_LIVECUP;
    WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_BTLREC );
    break;
  case BATTLE_CHAMPIONSHIP_CORE_RET_LIVERECPLAY: //LIVE�p�^��Đ��֍s��
    p_wk->type  = WIFIBATTLEMATCH_TYPE_LIVECUP;
    WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_RECPLAY );
    break;
  default:
    GF_ASSERT( 0 );
  }

  GFL_STD_MemClear( &p_wk->btl_score, sizeof(BATTLEMATCH_BATTLE_SCORE) );

	GFL_HEAP_FreeMemory( p_param );

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�}�b�`���O�R�A�̈���	�쐬
 *
 *	@param	HEAPID heapID			�q�[�vID
 *	@param	*p_wk_adrs				���[�N
 *
 *	@return	����
 */
//-----------------------------------------------------------------------------
static void *WBM_CORE_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs )
{	

  WIFIBATTLEMATCH_CORE_PARAM  *p_param;
  WIFIBATTLEMATCH_SYS         *p_wk     = p_wk_adrs;

  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_CORE_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_CORE_PARAM) );
	p_param->p_param	      = &p_wk->param;
  p_param->mode           = p_wk->core_mode;
  p_param->retmode        = p_wk->type - WIFIBATTLEMATCH_TYPE_RNDRATE;
  p_param->p_player_data  = p_wk->p_player_data;
  p_param->p_enemy_data   = p_wk->p_enemy_data;
  p_param->p_svl_result   = &p_wk->svl_result;
  p_param->p_server_time  = &p_wk->server_time;
  p_param->p_record_data  = &p_wk->record_data;
  p_param->p_recv_data    = &p_wk->recv_data;
  p_param->cp_btl_score   = &p_wk->btl_score;
  p_param->p_gpf_data     = &p_wk->gpf_data;
  p_param->p_sake_data    = &p_wk->sake_data;
  { 
    BATTLEMATCH_DATA  *p_btlmatch_sv  = SaveData_GetBattleMatch( GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data ) );
    p_param->p_rndmatch     =  BATTLEMATCH_GetRndMatch( p_btlmatch_sv );
  }

  if( PMSND_GetBGMsoundNo() != WBM_SND_SEQ_MAIN )
  { 
    PMSND_PlayBGM( WBM_SND_SEQ_MAIN );
  }
		
	return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�}�b�`���O�R�A�̈���	�j��
 *
 *	@param	void *p_param_adrs				����
 *	@param	*p_wk_adrs								���[�N
 */
//-----------------------------------------------------------------------------
static BOOL WBM_CORE_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs )
{	
  WIFIBATTLEMATCH_SYS         *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_param_adrs;

  //���[�h����
  switch( p_param->retmode )
  { 
  case WIFIBATTLEMATCH_CORE_RETMODE_RATE:  //���[�e�B���O���[�h
    p_wk->type  = WIFIBATTLEMATCH_TYPE_RNDRATE;
    break;
  case WIFIBATTLEMATCH_CORE_RETMODE_FREE:  //�t���[���[�h
    p_wk->type  = WIFIBATTLEMATCH_TYPE_RNDFREE;
    break;
  case WIFIBATTLEMATCH_CORE_RETMODE_WIFI: //WIFI���
    break;
  case WIFIBATTLEMATCH_CORE_RETMODE_NONE:  //�Ȃɂ��I�΂��ɏI��
    p_wk->type  = WIFIBATTLEMATCH_TYPE_RNDFREE;
    break;
  }

  //���ւ̍s����
  switch( p_param->result )
  { 
  case WIFIBATTLEMATCH_CORE_RESULT_NEXT_BATTLE:
    WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_POKELIST );
    break;
    
  case WIFIBATTLEMATCH_CORE_RESULT_NEXT_REC:
    WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_BTLREC );
    break;
  
  case WIFIBATTLEMATCH_CORE_RESULT_ERR_NEXT_LOGIN:
    p_wk->is_err_return_login = TRUE;
    WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_LOGIN );
    break;

  case WIFIBATTLEMATCH_CORE_RESULT_FINISH:
    WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_LOGOUT );
    break;

  default:
    GF_ASSERT( 0 );
  }

	GFL_HEAP_FreeMemory( p_param );

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���X�g�̈���	�쐬
 *
 *	@param	HEAPID heapID			�q�[�vID
 *	@param	*p_wk_adrs				���[�N
 *
 *	@return	����
 */
//-----------------------------------------------------------------------------
static void *POKELIST_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SUBPROC_PARAM    *p_param;
  WIFIBATTLEMATCH_SYS               *p_wk   = p_wk_adrs;
  int reg_no;

  //�|�P�p�[�e�B�̎󂯎��͓�����ADD���Ă邾���Ȃ̂ŁA
  //���񂱂��ŏ���������
  PokeParty_InitWork( p_wk->p_player_btl_party );
  PokeParty_InitWork( p_wk->p_enemy_btl_party );


  //�����쐬
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_SUBPROC_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_SUBPROC_PARAM) );

  if( p_wk->type == WIFIBATTLEMATCH_TYPE_RNDRATE
      || p_wk->type == WIFIBATTLEMATCH_TYPE_RNDFREE )
  { 
    //�����_���}�b�`�͋K��̃��M�����[�V�������g��

    switch( p_wk->param.btl_rule  )
    { 
    case WIFIBATTLEMATCH_BTLRULE_SINGLE:
      reg_no  = REG_RND_SINGLE;
      break;
    case WIFIBATTLEMATCH_BTLRULE_DOUBLE:
      reg_no  = REG_RND_DOUBLE;
      break;
    case WIFIBATTLEMATCH_BTLRULE_TRIPLE:
      reg_no  = REG_RND_TRIPLE;
      break;
    case WIFIBATTLEMATCH_BTLRULE_ROTATE:
      reg_no  = REG_RND_ROTATION;
      break;
    case WIFIBATTLEMATCH_BTLRULE_SHOOTER:
      reg_no  = REG_RND_TRIPLE_SHOOTER;
      break;
    default:
      GF_ASSERT(0);
    }

    p_param->regulation = (REGULATION*)PokeRegulation_LoadDataAlloc( reg_no, heapID );
  }
  else if( p_wk->type == WIFIBATTLEMATCH_TYPE_WIFICUP )
  { 
    //WIFI����WIFI���p���M�����[�V�������g��

    SAVE_CONTROL_WORK *p_sv       = GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data );
    REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
    REGULATION_CARDDATA *p_reg    = RegulationSaveData_GetRegulationCard( p_reg_sv, REGULATION_CARD_TYPE_WIFI );

    p_param->regulation       = RegulationData_GetRegulation( p_reg );
  }
  else if( p_wk->type == WIFIBATTLEMATCH_TYPE_LIVECUP )
  { 
    //LIVE����LIVE���p���M�����[�V�������g��

    SAVE_CONTROL_WORK *p_sv       = GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data );
    REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
    REGULATION_CARDDATA *p_reg    = RegulationSaveData_GetRegulationCard( p_reg_sv, REGULATION_CARD_TYPE_LIVE );

    p_param->regulation       = RegulationData_GetRegulation( p_reg );
  }
  else
  { 
    GF_ASSERT(0);
  }

  p_param->p_select_party   = p_wk->p_player_btl_party;
  p_param->gameData         = p_wk->param.p_game_data;

  //�����̃f�[�^
  { 
    int i;
    WIFIBATTLEMATCH_ENEMYDATA *p_player;
    POKEPARTY *p_party;
    POKEMON_PARAM *pp;
    p_player = p_wk->p_player_data;

    PokeParty_Copy((POKEPARTY*)p_player->pokeparty, p_wk->p_player_modify_party);
    PokeRegulation_ModifyLevelPokeParty( p_param->regulation, p_wk->p_player_modify_party );

    p_param->p_party  = p_wk->p_player_modify_party;

  }

  //�G�f�[�^
  { 
    WIFIBATTLEMATCH_ENEMYDATA *p_enemy;
    POKEPARTY *p_party;
    p_enemy = p_wk->p_enemy_data;

    PokeParty_Copy((POKEPARTY*)p_enemy->pokeparty, p_wk->p_enemy_modify_party);
    PokeRegulation_ModifyLevelPokeParty( p_param->regulation, p_wk->p_enemy_modify_party );

    p_param->enemyName      = MyStatus_GetMyName( (MYSTATUS*)p_enemy->mystatus );
    p_param->enemyPokeParty = p_wk->p_enemy_modify_party;
    p_param->enemySex       = MyStatus_GetMySex( (MYSTATUS*)p_enemy->mystatus );

  }

	return p_param;

}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�̈���	�j��
 *
 *	@param	void *p_param_adrs				����
 *	@param	*p_wk_adrs								���[�N
 */
//-----------------------------------------------------------------------------
static BOOL POKELIST_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_SUBPROC_PARAM   *p_param  = p_param_adrs;

  if( GFL_NET_GetNETInitStruct()->bNetType == GFL_NET_TYPE_IRC )
  { 

    if( p_param->result == WIFIBATTLEMATCH_SUBPROC_RESULT_SUCCESS )
    { 
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_LISTAFTER );
    }
    else if( p_param->result == WIFIBATTLEMATCH_SUBPROC_RESULT_ERROR_RETURN_LIVE )
    { 
      p_wk->btl_score.is_error  = TRUE;
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_MAINMENU );
    }
  }
  else
  { 
    if( p_param->result == WIFIBATTLEMATCH_SUBPROC_RESULT_SUCCESS )
    { 
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_LISTAFTER );
    }
    else if( p_param->result == WIFIBATTLEMATCH_SUBPROC_RESULT_ERROR_NEXT_LOGIN )
    { 
      p_wk->is_err_return_login = TRUE;
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_LOGIN );
    }
  }

  //�����_���}�b�`�ł̓��M�����[�V������ALLOC�����̂�
  //�������
  if( p_wk->type == WIFIBATTLEMATCH_TYPE_RNDRATE
      || p_wk->type == WIFIBATTLEMATCH_TYPE_RNDFREE )
  { 
    GFL_HEAP_FreeMemory( p_param->regulation );
  }
  GFL_HEAP_FreeMemory( p_param );

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���̈���	�쐬
 *
 *	@param	HEAPID heapID			�q�[�vID
 *	@param	*p_wk_adrs				���[�N
 *
 *	@return	����
 */
//-----------------------------------------------------------------------------
static void *BATTLE_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_BATTLELINK_PARAM  *p_param;
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  REGULATION* p_reg;
  BOOL is_alloc = FALSE;
  
  //�f���o�g���ڑ����[�N
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_BATTLELINK_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_BATTLELINK_PARAM) );
  p_param->p_gamedata  = p_wk->param.p_game_data;

  //�^��o�b�t�@���N���A
  BattleRec_DataClear();

  //�f���p�����[�^
  p_param->p_demo_param = GFL_HEAP_AllocMemory( heapID, sizeof(COMM_BTL_DEMO_PARAM) );
	GFL_STD_MemClear( p_param->p_demo_param, sizeof(COMM_BTL_DEMO_PARAM) );
  p_param->p_demo_param->record = GAMEDATA_GetRecordPtr( p_wk->param.p_game_data );

  //�f���p�����[�^�ւ̐ݒ�
  //����
  {
    COMM_BTL_DEMO_TRAINER_DATA *p_tr;
    p_tr  = &p_param->p_demo_param->trainer_data[ COMM_BTL_DEMO_TRDATA_A ];
    p_tr->mystatus  = (MYSTATUS*)p_wk->p_player_data->mystatus;
    p_tr->party     = p_wk->p_player_btl_party;
    p_tr->server_version  = p_wk->p_player_data->btl_server_version;
  }
  //����
  { 
    COMM_BTL_DEMO_TRAINER_DATA *p_tr;
    p_tr  = &p_param->p_demo_param->trainer_data[ COMM_BTL_DEMO_TRDATA_B ];
    p_tr->mystatus  = (MYSTATUS*)p_wk->p_enemy_data->mystatus;
    p_tr->party     = p_wk->p_enemy_btl_party;
    p_tr->server_version  = p_wk->p_enemy_data->btl_server_version;
  }

  //�o�g���ݒ�p�����[�^
  p_param->p_btl_setup_param	= GFL_HEAP_AllocMemory( heapID, sizeof(BATTLE_SETUP_PARAM) );
	GFL_STD_MemClear( p_param->p_btl_setup_param, sizeof(BATTLE_SETUP_PARAM) );

  GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );

	//�����_���o�g���̃o�g���ݒ�
  if( p_wk->type == WIFIBATTLEMATCH_TYPE_RNDRATE
      ||  p_wk->type == WIFIBATTLEMATCH_TYPE_RNDFREE )
  { 
    int reg_no;


    switch( p_wk->param.btl_rule )
    {
    case WIFIBATTLEMATCH_BTLRULE_SINGLE:    ///< �V���O��
      reg_no  = REG_RND_SINGLE;
      BTL_SETUP_Single_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case WIFIBATTLEMATCH_BTLRULE_DOUBLE:    ///< �_�u��
      reg_no  = REG_RND_DOUBLE;
      BTL_SETUP_Double_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case WIFIBATTLEMATCH_BTLRULE_TRIPLE:    ///< �g���v��
      reg_no  = REG_RND_TRIPLE;
      BTL_SETUP_Triple_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case WIFIBATTLEMATCH_BTLRULE_ROTATE:  ///< ���[�e�[�V����
      reg_no  = REG_RND_ROTATION;
      BTL_SETUP_Rotation_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;
    case WIFIBATTLEMATCH_BTLRULE_SHOOTER:  ///< �V���[�^�[
      reg_no  = REG_RND_TRIPLE_SHOOTER;
      BTL_SETUP_Triple_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      //�V���[�^�[�p�ݒ�͉����ōs���Ă���
      break;
    }

    p_reg = (REGULATION*)PokeRegulation_LoadDataAlloc( reg_no, heapID );
    is_alloc  = TRUE;
  }
  //WiFI���̃o�g���ݒ�
  else if( p_wk->type == WIFIBATTLEMATCH_TYPE_WIFICUP )
  { 
    SAVE_CONTROL_WORK *p_sv       = GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data );
    REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
    REGULATION_CARDDATA *p_reg_card    = RegulationSaveData_GetRegulationCard( p_reg_sv, REGULATION_CARD_TYPE_WIFI );
    p_reg        = RegulationData_GetRegulation( p_reg_card );

    switch( Regulation_GetParam( p_reg, REGULATION_BATTLETYPE ) )
    {
    case REGULATION_BATTLE_SINGLE:    ///< �V���O��
      BTL_SETUP_Single_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case REGULATION_BATTLE_DOUBLE:    ///< �_�u��
      BTL_SETUP_Double_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case REGULATION_BATTLE_TRIPLE:    ///< �g���v��
      BTL_SETUP_Triple_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case REGULATION_BATTLE_ROTATION:  ///< ���[�e�[�V����
      BTL_SETUP_Rotation_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    default:
      GF_ASSERT( 0 );
      //�O�̂��߃V���O���ɂ��Ă���
      BTL_SETUP_Single_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
    }
  }
  //LIVE���̃o�g���ݒ�
  else if( p_wk->type == WIFIBATTLEMATCH_TYPE_LIVECUP )
  { 
    SAVE_CONTROL_WORK *p_sv       = GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data );
    REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
    REGULATION_CARDDATA *p_reg_card    = RegulationSaveData_GetRegulationCard( p_reg_sv, REGULATION_CARD_TYPE_LIVE );
    p_reg        = RegulationData_GetRegulation( p_reg_card );

    switch( Regulation_GetParam( p_reg, REGULATION_BATTLETYPE ) )
    {
      //@todo BTL_COMM_WIFI?
    case REGULATION_BATTLE_SINGLE:    ///< �V���O��
      BTL_SETUP_Single_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case REGULATION_BATTLE_DOUBLE:    ///< �_�u��
      BTL_SETUP_Double_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case REGULATION_BATTLE_TRIPLE:    ///< �g���v��
      BTL_SETUP_Triple_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case REGULATION_BATTLE_ROTATION:  ///< ���[�e�[�V����
      BTL_SETUP_Rotation_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    default:
      GF_ASSERT( 0 );
      //�O�̂��߃V���O���ɂ��Ă���
      BTL_SETUP_Single_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;
    }
  }
  else 
  { 
    GF_ASSERT(0);
  }

  //�o�g���^�C�v�ݒ�
  if( p_wk->type == WIFIBATTLEMATCH_TYPE_RNDFREE )
  { 
    //�����_���}�b�`�F�t���[
    p_param->p_demo_param->battle_mode = 
      BATTLE_MODE_RANDOM_FREE_SINGLE + p_wk->param.btl_rule;
  }
  else if( p_wk->type == WIFIBATTLEMATCH_TYPE_RNDRATE )
  { 
    //�����_���}�b�`�F���[�e�B���O
    p_param->p_demo_param->battle_mode = 
      BATTLE_MODE_RANDOM_RATING_SINGLE + p_wk->param.btl_rule;
  }
  else
  { 
    BOOL  is_shooter  = Regulation_GetParam( p_reg, REGULATION_SHOOTER );
    REGULATION_BATTLE_TYPE  battle_type = Regulation_GetParam( p_reg, REGULATION_BATTLETYPE );

    p_param->p_demo_param->battle_mode = battle_type * 2 + is_shooter;
  }

  //�Ȃ̐ݒ�
  if( p_wk->type == WIFIBATTLEMATCH_TYPE_RNDRATE
      ||  p_wk->type == WIFIBATTLEMATCH_TYPE_RNDFREE )
  { 
    p_param->p_btl_setup_param->musicDefault  = WBM_SND_SEQ_BATTLE_RND;
    p_param->p_btl_setup_param->musicPinch    = WBM_SND_SEQ_BATTLE_PINCH;
  }
  else if( p_wk->type == WIFIBATTLEMATCH_TYPE_WIFICUP )
  { 
    p_param->p_btl_setup_param->musicDefault  = WBM_SND_SEQ_BATTLE_WIFI;
    p_param->p_btl_setup_param->musicPinch    = WBM_SND_SEQ_BATTLE_PINCH;
  }
  else if( p_wk->type == WIFIBATTLEMATCH_TYPE_LIVECUP )
  { 
    p_param->p_btl_setup_param->musicDefault  = WBM_SND_SEQ_BATTLE_LIVE;
    p_param->p_btl_setup_param->musicPinch    = WBM_SND_SEQ_BATTLE_PINCH;
  }

  //�|�P�����ݒ�
  BATTLE_PARAM_SetPokeParty( p_param->p_btl_setup_param, p_wk->p_player_btl_party, BTL_CLIENT_PLAYER );

  //���M�����[�V�����̓��e��K�p
  BATTLE_PARAM_SetRegulation( p_param->p_btl_setup_param, p_reg, GFL_HEAP_LOWID( heapID ) );

  OS_TFPrintf( 3, "vs %d\n", p_param->p_btl_setup_param->LimitTimeGame );
  OS_TFPrintf( 3, "cmd %d\n",  p_param->p_btl_setup_param->LimitTimeCommand );

  //�^�揀��
  BTL_SETUP_AllocRecBuffer( p_param->p_btl_setup_param, heapID );

  GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );


  if( is_alloc )
  { 
    GFL_HEAP_FreeMemory( p_reg );
  }

  //�f���p�����[�^
	return p_param;

}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���̈���	�j��
 *
 *	@param	void *p_param_adrs				����
 *	@param	*p_wk_adrs								���[�N
 */
//-----------------------------------------------------------------------------
static BOOL BATTLE_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_BATTLELINK_PARAM  *p_param  = p_param_adrs;
  BATTLE_SETUP_PARAM  *p_btl_param  = p_param->p_btl_setup_param;
  WIFIBATTLEMATCH_BATTLELINK_RESULT result  = p_param->result;

  //�󂯎��
  p_wk->btl_score.rule  = p_btl_param->rule;
  p_wk->btl_score.result  = p_btl_param->result;
  p_wk->btl_score.is_dirty  = p_btl_param->cmdIllegalFlag;
  p_wk->btl_score.result  = p_btl_param->result;
  { 
    int i;
    u32 now_hp_all;
    u32 max_hp_all;
    POKEPARTY *p_party  = p_param->p_btl_setup_param->party[ BTL_CLIENT_ENEMY1 ];

    p_wk->btl_score.enemy_rest_poke = PokeParty_GetPokeCountBattleEnable( p_party );
    
    now_hp_all  = 0;
    max_hp_all  = 0;
    for( i = 0; i < PokeParty_GetPokeCount( p_party ); i++ )
    { 
      POKEMON_PARAM *p_pp = PokeParty_GetMemberPointer( p_party, i );
      if( PP_Get( p_pp, ID_PARA_poke_exist, NULL ) && !PP_Get( p_pp, ID_PARA_tamago_flag, NULL ) )
      { 
        now_hp_all  += PP_Get( p_pp, ID_PARA_hp, NULL );
        max_hp_all  += PP_Get( p_pp, ID_PARA_hpmax, NULL );
      }
    }

    p_wk->btl_score.enemy_rest_hp   = 100 * now_hp_all / max_hp_all;
  }

  OS_FPrintf( 3, "�o�g������ %d \n", p_wk->btl_score.result);

  //�^����Ƀo�g������ݒ�
  BattleRec_LoadToolModule();
  BattleRec_StoreSetupParam( p_param->p_btl_setup_param );
  BattleRec_UnloadToolModule();

  //��т��c��
  if( p_wk->type == WIFIBATTLEMATCH_TYPE_WIFICUP )
  { 
    SAVE_CONTROL_WORK   *p_sv     = GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data );
    REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
    REGULATION_CARDDATA *p_reg_card= RegulationSaveData_GetRegulationCard( p_reg_sv, REGULATION_CARD_TYPE_WIFI );
    REGULATION          *p_reg    = RegulationData_GetRegulation( p_reg_card );
    u32                 cupNO     = Regulation_GetCardParam( p_reg_card, REGULATION_CARD_CUPNO );

    Util_SetRecordData( &p_wk->record_data, p_wk->p_player_btl_party, p_wk->p_enemy_btl_party, p_wk->p_enemy_data, p_reg, cupNO, p_btl_param->result, WIFIBATTLEMATCH_TYPE_WIFICUP );
  }
  else if( p_wk->type == WIFIBATTLEMATCH_TYPE_RNDRATE )
  { 
    REGULATION          *p_reg    = (REGULATION*)PokeRegulation_LoadDataAlloc( REG_RND_SINGLE + p_wk->param.btl_rule, HEAPID_WIFIBATTLEMATCH_SYS );
    Util_SetRecordData( &p_wk->record_data, p_wk->p_player_btl_party, p_wk->p_enemy_btl_party, p_wk->p_enemy_data, p_reg, 0, p_btl_param->result, WIFIBATTLEMATCH_TYPE_RNDRATE );
    GFL_HEAP_FreeMemory( p_reg );
  }

  //�j��
  BATTLE_PARAM_Release( p_param->p_btl_setup_param );
	GFL_HEAP_FreeMemory( p_param->p_btl_setup_param );
	GFL_HEAP_FreeMemory( p_param->p_demo_param );
	GFL_HEAP_FreeMemory( p_param );

  //����PROC
  if( p_wk->type == WIFIBATTLEMATCH_TYPE_LIVECUP )
  { 
    switch( result )
    {
    case WIFIBATTLEMATCH_BATTLELINK_RESULT_SUCCESS:
      //���C�u�}�b�`�̓��C�����j���[���炭��̂Ń��C�u�܂����֖߂�
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_MAINMENU );
      break;
    default:
      p_wk->btl_score.is_error  = TRUE;
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_MAINMENU );
      break;
    }
  }
  else
  {
    //�����_���}�b�`��WIFI��CORE���炭��
    switch( result )
    {
    case WIFIBATTLEMATCH_BATTLELINK_RESULT_SUCCESS:
      p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE;
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_CORE );
      break;

    case WIFIBATTLEMATCH_BATTLELINK_RESULT_DISCONNECT:
      //�ؒf���ꂽ
      p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_CORE );
      break;

    case WIFIBATTLEMATCH_BATTLELINK_RESULT_EVIL:
      p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_CORE );
      break;
    }
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI���O�C���̈���	�쐬
 *
 *	@param	HEAPID heapID			�q�[�vID
 *	@param	*p_wk_adrs				���[�N
 *
 *	@return	����
 */
//-----------------------------------------------------------------------------
static void *LOGIN_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs )
{ 
  WIFILOGIN_PARAM *p_param;
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFILOGIN_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFILOGIN_PARAM) );

  p_param->gamedata = p_wk->param.p_game_data;
  p_param->bg       = WIFILOGIN_BG_NORMAL;
  p_param->display  = WIFILOGIN_DISPLAY_UP;
  p_param->pSvl     = &p_wk->svl_result;
  p_param->nsid     = WB_NET_WIFIMATCH;

  if( p_wk->is_err_return_login )
  { 
    p_wk->is_err_return_login = FALSE;
    p_param->mode     = WIFILOGIN_MODE_ERROR;
  }
  else
  { 
    p_param->mode     = WIFILOGIN_MODE_NORMAL;
  }

  return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���O�C���̈���	�j��
 *
 *	@param	void *p_param_adrs				����
 *	@param	*p_wk_adrs								���[�N
 */
//-----------------------------------------------------------------------------
static BOOL LOGIN_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  WIFILOGIN_PARAM  *p_param     = p_param_adrs;
  WIFILOGIN_RESULT  result      = p_param->result;

  GFL_HEAP_FreeMemory( p_param );

  switch( result )
  { 
  case WIFILOGIN_RESULT_LOGIN:
    p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_START;
    WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_CORE );
    break;

  case WIFILOGIN_RESULT_CANCEL:
    WBM_SYS_SUBPROC_End( p_subproc );
    break;
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���X�g��v���Z�X�̈���	�쐬
 *
 *	@param	HEAPID heapID			�q�[�vID
 *	@param	*p_wk_adrs				���[�N
 *
 *	@return	����
 */
//-----------------------------------------------------------------------------
static void *WBM_LISTAFTER_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_LISTAFTER_PARAM *p_param;
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_LISTAFTER_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_LISTAFTER_PARAM) );
  p_param->p_param        = &p_wk->param;
  p_param->p_player_btl_party = p_wk->p_player_btl_party;
  p_param->p_enemy_btl_party  = p_wk->p_enemy_btl_party;
  p_param->p_recv_data        = &p_wk->recv_data;

  if( p_wk->type == WIFIBATTLEMATCH_TYPE_LIVECUP )
  { 
    p_param->type = WIFIBATTLEMATCH_LISTAFTER_NETTYPE_IRC;
  }
  else
  { 
    p_param->type = WIFIBATTLEMATCH_LISTAFTER_NETTYPE_WIFI;
  }

  return p_param;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���X�g��v���Z�X�̈���	�j��
 *
 *	@param	void *p_param_adrs				����
 *	@param	*p_wk_adrs								���[�N
 */
//-----------------------------------------------------------------------------
static BOOL WBM_LISTAFTER_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS               *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_LISTAFTER_PARAM   *p_param  = p_param_adrs;
  WIFIBATTLEMATCH_LISTAFTER_RESULT  result    = p_param->result;

  GFL_HEAP_FreeMemory( p_param );

  if( GFL_NET_GetNETInitStruct()->bNetType == GFL_NET_TYPE_IRC )
  { 
    switch( result )
    { 
    case WIFIBATTLEMATCH_LISTAFTER_RESULT_SUCCESS:
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_BATTLE );
      break;
    case WIFIBATTLEMATCH_LISTAFTER_RESULT_ERROR_RETURN_LIVE:
      p_wk->btl_score.is_error  = TRUE;
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_MAINMENU );
      break;
    }
  }
  else
  { 
    switch( result )
    { 
    case WIFIBATTLEMATCH_LISTAFTER_RESULT_SUCCESS:
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_BATTLE );
      break;

    case WIFIBATTLEMATCH_LISTAFTER_RESULT_ERROR_NEXT_LOGIN:
      p_wk->is_err_return_login = TRUE;
      WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_LOGIN );
      break;
    }
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�^��v���Z�X�̈���	�쐬
 *
 *	@param	HEAPID heapID			�q�[�vID
 *	@param	*p_wk_adrs				���[�N
 *
 *	@return	����
 */
//-----------------------------------------------------------------------------
static void *WBM_BTLREC_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  BTL_REC_SEL_PARAM               *p_param;
  BATTLE_MODE                     battle_mode;

  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(BTL_REC_SEL_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(BTL_REC_SEL_PARAM) );

  //�o�g���^�C�v�ݒ�
  if( p_wk->type == WIFIBATTLEMATCH_TYPE_RNDFREE )
  { 
    //�����_���}�b�`�F�t���[
    battle_mode = BATTLE_MODE_RANDOM_FREE_SINGLE + p_wk->param.btl_rule;
  }
  else if( p_wk->type == WIFIBATTLEMATCH_TYPE_RNDRATE )
  { 
    //�����_���}�b�`�F���[�e�B���O
    battle_mode = BATTLE_MODE_RANDOM_RATING_SINGLE + p_wk->param.btl_rule;
  }
  else
  { 
    REGULATION_CARD_TYPE  type;
    if( p_wk->type == WIFIBATTLEMATCH_TYPE_WIFICUP )
    { 
      //WIFI���
      type  = REGULATION_CARD_TYPE_WIFI;
    }
    else if( p_wk->type == WIFIBATTLEMATCH_TYPE_LIVECUP )
    { 
      //LIVE���
      type  = REGULATION_CARD_TYPE_LIVE;
    }
    else
    { 
      GF_ASSERT(0);
    }

    { 
      SAVE_CONTROL_WORK   *p_sv       = GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data );
      REGULATION_SAVEDATA *p_reg_sv   = SaveData_GetRegulationSaveData( p_sv );
      REGULATION_CARDDATA *p_reg_card = RegulationSaveData_GetRegulationCard( p_reg_sv, type );
      REGULATION          *p_reg      = RegulationData_GetRegulation( p_reg_card );
      BOOL                is_shooter  = Regulation_GetParam( p_reg, REGULATION_SHOOTER );
      REGULATION_BATTLE_TYPE  battle_type  = Regulation_GetParam( p_reg, REGULATION_BATTLETYPE );

      battle_mode = battle_type * 2 + is_shooter;
    }
  }

  p_param->gamedata     = p_wk->param.p_game_data;
  p_param->b_rec        = TRUE;
  p_param->b_sync       = FALSE;
  p_param->battle_mode  = battle_mode;;
  p_param->fight_count  = 0;

  if( p_wk->p_enemy_data->btl_server_version != p_wk->p_player_data->btl_server_version )
  { 
    p_param->b_rec      = FALSE;
  }

  return p_param;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^��v���Z�X�̈���	�j��
 *
 *	@param	void *p_param_adrs				����
 *	@param	*p_wk_adrs								���[�N
 */
//-----------------------------------------------------------------------------
static BOOL WBM_BTLREC_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  BTL_REC_SEL_PARAM               *p_param  = p_param_adrs;

  GFL_HEAP_FreeMemory( p_param );

  //����PROC
  if( p_wk->type == WIFIBATTLEMATCH_TYPE_LIVECUP )
  { 
    //���C�u�}�b�`�̓��C�����j���[���炭��̂Ń��C�u�܂����֖߂�
    WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_MAINMENU );
  }
  else
  { 
    p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDREC;
    WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_CORE );
  }

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI���O�A�E�g�v���Z�X�̈���	�j��
 *
 *	@param	void *p_param_adrs				����
 *	@param	*p_wk_adrs								���[�N
 */
//-----------------------------------------------------------------------------
static void *LOGOUT_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  WIFILOGOUT_PARAM    *p_param;

  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFILOGOUT_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFILOGOUT_PARAM) );

  p_param->gamedata = p_wk->param.p_game_data;
  p_param->bg       = WIFILOGIN_BG_NORMAL;
  p_param->display  = WIFILOGIN_DISPLAY_UP;

  return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI���O�A�E�g�v���Z�X�̈���	�j��
 *
 *	@param	void *p_param_adrs				����
 *	@param	*p_wk_adrs								���[�N
 */
//-----------------------------------------------------------------------------
static BOOL LOGOUT_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  WIFILOGOUT_PARAM    *p_param  = p_param_adrs;

  switch( p_wk->type )
  { 
  case WIFIBATTLEMATCH_TYPE_WIFICUP:
    WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_MAINMENU );
    break;
  case WIFIBATTLEMATCH_TYPE_LIVECUP:
    GF_ASSERT(0);
    break;
  case WIFIBATTLEMATCH_TYPE_RNDRATE:
  case WIFIBATTLEMATCH_TYPE_RNDFREE:
    WBM_SYS_SUBPROC_End( p_subproc );
    break;
  }

  GFL_HEAP_FreeMemory( p_param );

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�^��Đ��v���Z�X�̈���	�j��
 *
 *	@param	void *p_param_adrs				����
 *	@param	*p_wk_adrs								���[�N
 */
//-----------------------------------------------------------------------------
static void *RECPLAY_AllocParam( WBM_SYS_SUBPROC_WORK *p_subproc,HEAPID heapID, void *p_wk_adrs )
{ 
  BATTLE_SETUP_PARAM  *p_param;
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  
  //�o�g���ݒ�p�����[�^
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(BATTLE_SETUP_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(BATTLE_SETUP_PARAM) );

  GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
  BATTLE_PARAM_Init( p_param );
  BTL_SETUP_InitForRecordPlay( p_param, p_wk->param.p_game_data, heapID );

  GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
  BattleRec_LoadToolModule();
  BattleRec_RestoreSetupParam( p_param, heapID );
  BattleRec_UnloadToolModule();

  GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );


  PMSND_StopBGM();
  PMSND_PlayBGM( p_param->musicDefault );

  //�f���p�����[�^
	return p_param;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^��Đ��v���Z�X�̈���  �j��
 *
 *	@param	void *p_param_adrs				����
 *	@param	*p_wk_adrs								���[�N
 */
//-----------------------------------------------------------------------------
static BOOL RECPLAY_FreeParam( WBM_SYS_SUBPROC_WORK *p_subproc,void *p_param_adrs, void *p_wk_adrs )
{ 
  BATTLE_SETUP_PARAM  *p_param  = p_param_adrs;
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  
  PMSND_StopBGM();

  GFL_HEAP_FreeMemory( p_param->playerStatus[ BTL_CLIENT_PLAYER ] );  //�v���C���[��MySatus�͊J������Ȃ��̂�
  BTL_SETUP_QuitForRecordPlay( p_param );
  GFL_HEAP_FreeMemory( p_param );

  GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );

  WBM_SYS_SUBPROC_CallProc( p_subproc, SUBPROCID_MAINMENU );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�o�b�t�@�쐬
 *
 *	@param	WIFIBATTLEMATCH_SYS *p_wk ���[�N
 *	@param  heapID                    heapID
 */
//-----------------------------------------------------------------------------
static void DATA_CreateBuffer( WIFIBATTLEMATCH_SYS *p_wk, HEAPID heapID )
{ 
  p_wk->p_player_data = GFL_HEAP_AllocMemory( heapID, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
  GFL_STD_MemClear( p_wk->p_player_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
  p_wk->p_enemy_data  = GFL_HEAP_AllocMemory( heapID, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
  GFL_STD_MemClear( p_wk->p_enemy_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );

}
//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�o�b�t�@�j��
 *
 *	@param	WIFIBATTLEMATCH_SYS *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void DATA_DeleteBuffer( WIFIBATTLEMATCH_SYS *p_wk )
{ 
  GFL_HEAP_FreeMemory( p_wk->p_player_data );
  GFL_HEAP_FreeMemory( p_wk->p_enemy_data );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�쐬
 *
 *	@param	WIFIBATTLEMATCH_RECORD_DATA *p_data ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_SetRecordData( WIFIBATTLEMATCH_RECORD_DATA *p_data, const POKEPARTY *cp_my_poke, const POKEPARTY *cp_you_poke, const WIFIBATTLEMATCH_ENEMYDATA *cp_you_data, const REGULATION *cp_reg, u32 cupNO, BtlResult result, WIFIBATTLEMATCH_TYPE type )
{ 
  int i;
  GFL_STD_MemClear( p_data, sizeof(WIFIBATTLEMATCH_RECORD_DATA) );

  //�ΐ푊��̃v���t�B�[��
  p_data->your_gamesyncID = MyStatus_GetProfileID( (MYSTATUS*)cp_you_data->mystatus );
  p_data->your_profileID  = cp_you_data->profileID;

  //�ΐ푊��̃|�P����
  for( i = 0; i < PokeParty_GetPokeCount(cp_you_poke); i++ )
  { 
    POKEMON_PARAM *p_pp = PokeParty_GetMemberPointer( cp_you_poke, i );
    if( PP_Get( p_pp, ID_PARA_poke_exist, NULL ) )
    { 
      p_data->your_monsno[i] = PP_Get( p_pp, ID_PARA_monsno, NULL );
      p_data->your_form[i]   = PP_Get( p_pp, ID_PARA_form_no, NULL );
      p_data->your_lv[i]     = PP_Get( p_pp, ID_PARA_level, NULL );
      p_data->your_sex[i]    = PP_Get( p_pp, ID_PARA_sex, NULL );
    }
  }

  //�����̃|�P����
  for( i = 0; i < PokeParty_GetPokeCount(cp_my_poke); i++ )
  { 
    POKEMON_PARAM *p_pp = PokeParty_GetMemberPointer( cp_my_poke, i );
    if( PP_Get( p_pp, ID_PARA_poke_exist, NULL ) )
    { 
      p_data->my_monsno[i] = PP_Get( p_pp, ID_PARA_monsno, NULL );
      p_data->my_form[i]   = PP_Get( p_pp, ID_PARA_form_no, NULL );
      p_data->my_lv[i]     = PP_Get( p_pp, ID_PARA_level, NULL );
      p_data->my_sex[i]    = PP_Get( p_pp, ID_PARA_sex, NULL );
    }
  }

  //�ΐ���
  { 
    BOOL ret;
    RTCDate date;
    RTCTime time;
    ret = DWC_GetDateTime( &date, &time );
    GF_ASSERT( ret );

    p_data->year  = date.year;
    p_data->month = date.month;
    p_data->day   = date.day;
    p_data->hour  = time.hour;
    p_data->minute= time.minute;
  }
  p_data->cupNO   = cupNO;
  p_data->result  = result;
  p_data->btl_type= Regulation_GetParam( cp_reg, REGULATION_BATTLETYPE );
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �T�u�v���Z�X
 *				    �E�v���Z�X���s��������V�X�e��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#define WBM_SYS_SUBPROC_END   (0xFF)
//-------------------------------------
///	���[�N
//=====================================
struct _WBM_SYS_SUBPROC_WORK
{
	GFL_PROCSYS			  *p_procsys;
	void						  *p_proc_param;

	HEAPID					  heapID;
	void						  *p_wk_adrs;
	const WBM_SYS_SUBPROC_DATA			*cp_procdata_tbl;
  u32               tbl_len;
  u8                pre_procID;
	u8							  next_procID;
	u8							  now_procID;
	u8							  seq;

  GFL_PROC_MAIN_STATUS  status;
} ;
//-------------------------------------
///	�O�����J
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	WBM_SYS_SUBPROC�V�X�e��	������
 *
 *	@param	WBM_SYS_SUBPROC_WORK *p_wk	���[�N
 *	@param	cp_procdata_tbl			�v���Z�X�ڑ��e�[�u��
 *	@param	void *p_wk_adrs			�A���b�N�֐��Ɖ���֐��ɓn�����[�N
 *	@param	heapID							�V�X�e���\�z�p�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static WBM_SYS_SUBPROC_WORK * WBM_SYS_SUBPROC_Init( const WBM_SYS_SUBPROC_DATA *cp_procdata_tbl, u32 tbl_len, void *p_wk_adrs, HEAPID heapID )
{	
  WBM_SYS_SUBPROC_WORK *p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(WBM_SYS_SUBPROC_WORK ) );
	GFL_STD_MemClear( p_wk, sizeof(WBM_SYS_SUBPROC_WORK) );
	p_wk->p_procsys				= GFL_PROC_LOCAL_boot( heapID );
	p_wk->p_wk_adrs				= p_wk_adrs;
	p_wk->cp_procdata_tbl	= cp_procdata_tbl;
  p_wk->tbl_len         = tbl_len;
	p_wk->heapID					= heapID;

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	WBM_SYS_SUBPROC�V�X�e��	�j��
 *
 *	@param	WBM_SYS_SUBPROC_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void WBM_SYS_SUBPROC_Exit( WBM_SYS_SUBPROC_WORK *p_wk )
{	
	GF_ASSERT( p_wk->p_proc_param == NULL );

	GFL_PROC_LOCAL_Exit( p_wk->p_procsys );

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	WBM_SYS_SUBPROC�V�X�e��	���C������
 *
 *	@param	WBM_SYS_SUBPROC_WORK *p_wk	���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��	FALSE�Ȃ��PROC�����݂���
 */
//-----------------------------------------------------------------------------
static BOOL WBM_SYS_SUBPROC_Main( WBM_SYS_SUBPROC_WORK *p_wk )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_ALLOC_PARAM,
		SEQ_MAIN,
		SEQ_FREE_PARAM,
    SEQ_NEXT,
		SEQ_END,
	};

	switch( p_wk->seq )
	{	
	case SEQ_INIT:
    p_wk->pre_procID  = p_wk->now_procID;
		p_wk->now_procID	= p_wk->next_procID;
		p_wk->seq	= SEQ_ALLOC_PARAM;
		break;

	case SEQ_ALLOC_PARAM:
		//�v���Z�X�����쐬�֐�������ΌĂяo��
		if( p_wk->cp_procdata_tbl[ p_wk->now_procID ].alloc_func )
		{	
			p_wk->p_proc_param	= p_wk->cp_procdata_tbl[ p_wk->now_procID ].alloc_func(
          p_wk, 
					p_wk->heapID, p_wk->p_wk_adrs );
		}
		else
		{	
			p_wk->p_proc_param	= NULL;
		}

		//�v���b�N�Ăяo��
		GFL_PROC_LOCAL_CallProc( p_wk->p_procsys, p_wk->cp_procdata_tbl[	p_wk->now_procID ].ov_id,
					p_wk->cp_procdata_tbl[	p_wk->now_procID ].cp_procdata, p_wk->p_proc_param );

		p_wk->seq	= SEQ_MAIN;
		break;

	case SEQ_MAIN:
		{	
			p_wk->status	= GFL_PROC_LOCAL_Main( p_wk->p_procsys );
			if( GFL_PROC_MAIN_NULL == p_wk->status )
			{	
				p_wk->seq	= SEQ_FREE_PARAM;
			}
		}
		break;

	case SEQ_FREE_PARAM:
		//�v���Z�X�����j���֐��Ăяo��
		if( p_wk->cp_procdata_tbl[	p_wk->now_procID ].free_func )
		{	
			if( p_wk->cp_procdata_tbl[	p_wk->now_procID ].free_func( p_wk, p_wk->p_proc_param, p_wk->p_wk_adrs ) )
      { 
        p_wk->p_proc_param	= NULL;
				p_wk->seq	= SEQ_NEXT;
      }
		}
    else
    { 
				p_wk->seq	= SEQ_NEXT;
    }
    break;

  case SEQ_NEXT:
		//�������̃v���Z�X������ΌĂяo��
		//�Ȃ���ΏI��
		if( p_wk->now_procID == p_wk->next_procID )
		{	
      GF_ASSERT_MSG( 0, "����PROC���w�肳��Ă��Ȃ�\n" );
		}
    if( p_wk->next_procID == WBM_SYS_SUBPROC_END )
    { 
			p_wk->seq	= SEQ_END;
    }
		else
		{	
			p_wk->seq	= SEQ_INIT;
		}
		break;

	case SEQ_END:
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WBM_SYS_SUBPROC�V�X�e�� PROC��Ԃ��擾
 *
 *	@param	const WBM_SYS_SUBPROC_WORK *cp_wk   ���[�N
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static GFL_PROC_MAIN_STATUS WBM_SYS_SUBPROC_GetStatus( const WBM_SYS_SUBPROC_WORK *cp_wk )
{ 
  return cp_wk->status;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WBM_SYS_SUBPROC�V�X�e�� �O���PROCID���擾
 *
 *	@param	const WBM_SYS_SUBPROC_WORK *cp_wk   ���[�N
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static u8 WBM_SYS_SUBPROC_GetPreProcID( const WBM_SYS_SUBPROC_WORK *cp_wk )
{ 
  return cp_wk->pre_procID;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WBM_SYS_SUBPROC�V�X�e��	�v���Z�X���N�G�X�g
 *
 *	@param	WBM_SYS_SUBPROC_WORK *p_wk	���[�N
 *	@param	proc_id							�Ăԃv���Z�XID
 *
 */
//-----------------------------------------------------------------------------
static void WBM_SYS_SUBPROC_CallProc( WBM_SYS_SUBPROC_WORK *p_wk, u32 procID )
{	
	p_wk->next_procID	= procID;
}


//----------------------------------------------------------------------------
/**
 *	@brief	WBM_SYS_SUBPROC�V�X�e��	�v���Z�X�̌q����I��
 *
 *	@param	WBM_SYS_SUBPROC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void WBM_SYS_SUBPROC_End( WBM_SYS_SUBPROC_WORK *p_wk )
{ 
  p_wk->next_procID = WBM_SYS_SUBPROC_END;
}
