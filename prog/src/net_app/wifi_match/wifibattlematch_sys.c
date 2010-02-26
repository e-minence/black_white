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

//�e�v���Z�X
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"
#include "title/title.h"
#include "wifibattlematch_battle.h"
#include "net_app/btl_rec_sel.h"
#include "wifibattlematch_utilproc.h"
#include "wifibattlematch_subproc.h"

//�Z�[�u�f�[�^
#include "savedata/battle_box_save.h"
#include "savedata/wifihistory.h"
#include "savedata/my_pms_data.h"

//�����̃��W���[��
#include "wifibattlematch_core.h"
#include "wifibattlematch_data.h"
#include "wifibattlematch_snd.h"

//�O�����J
#include "net_app/wifibattlematch.h"

//-------------------------------------
///	DEBUG
//=====================================
#ifdef PM_DEBUG
#endif //PM_DEBUG

//-------------------------------------
///	PROC�G�N�X�^�[��
//=====================================

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define WIFIBATTLEMATCH_MEMBER_NUM  (2)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�T�u�v���b�N�ړ�
//=====================================
typedef void *(*SUBPROC_ALLOC_FUNCTION)( HEAPID heapID, void *p_wk_adrs );
typedef BOOL (*SUBPROC_FREE_FUNCTION)( void *p_param, void *p_wk_adrs );
typedef struct 
{
	FSOverlayID							ov_id;
	const GFL_PROC_DATA			*cp_procdata;
	SUBPROC_ALLOC_FUNCTION	alloc_func;
	SUBPROC_FREE_FUNCTION		free_func;
} SUBPROC_DATA;
typedef struct {
	GFL_PROCSYS			*p_procsys;
	void						*p_proc_param;
	SUBPROC_DATA		*p_data;

	HEAPID					heapID;
	void						*p_wk_adrs;
	const SUBPROC_DATA			*cp_procdata_tbl;

	u8							next_procID;
	u8							now_procID;
	u16							seq;

  GFL_PROC_MAIN_STATUS  status;
} SUBPROC_WORK;
//-------------------------------------
///	�V�X�e�����[�N
//=====================================
typedef struct
{ 
  BOOL                        is_create_gamedata;
  WIFIBATTLEMATCH_PARAM       param;
  SUBPROC_WORK                subproc;

  //�R�A���[�h
  WIFIBATTLEMATCH_CORE_MODE     core_mode;
  WIFIBATTLEMATCH_CORE_RETMODE  core_ret;

  //�o�g���̌���
  BtlResult                   btl_result;
  BtlRule                     btl_rule;

  POKEPARTY                   *p_player_btl_party; //�����Ō��߂��p�[�e�B
  POKEPARTY                   *p_enemy_btl_party; //����̌��߂��p�[�e�B

  //�ȉ��V�X�e���w�ɒu���Ă����f�[�^
  WIFIBATTLEMATCH_ENEMYDATA   *p_player_data;
  WIFIBATTLEMATCH_ENEMYDATA   *p_enemy_data;

  DWCSvlResult                svl_result;

#if 0
  DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA *p_gpf_data;
  WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA   *p_sake_data;
#endif

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
///	�T�u�v���Z�X
//=====================================
static void SUBPROC_Init( SUBPROC_WORK *p_wk, const SUBPROC_DATA *cp_procdata_tbl, void *p_wk_adrs, HEAPID heapID );
static BOOL SUBPROC_Main( SUBPROC_WORK *p_wk );
static GFL_PROC_MAIN_STATUS SUBPROC_GetStatus( const SUBPROC_WORK *cp_wk );
static void SUBPROC_Exit( SUBPROC_WORK *p_wk );
static void SUBPROC_CallProc( SUBPROC_WORK *p_wk, u32 procID );

//-------------------------------------
///	�T�u�v���Z�X�p�����̉���A�j���֐�
//=====================================
//�}�b�`���O���C���v���Z�X
static void *WBM_CORE_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL WBM_CORE_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//���X�g�{�X�e�[�^�X
static void *POKELIST_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL POKELIST_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//�o�g���{�f��
static void *BATTLE_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL BATTLE_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//WIFI���O�C��
static void *LOGIN_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL LOGIN_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//���X�g�{�X�e�[�^�X�`�o�g���{�f���ւ̂Ȃ��v���Z�X
static void *WBM_LISTAFTER_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL WBM_LISTAFTER_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//�^��
static void *WBM_BTLREC_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL WBM_BTLREC_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//WIFI���O�A�E�g
static void *LOGOUT_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL LOGOUT_FreeParam( void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	�f�[�^�o�b�t�@�쐬
//=====================================
static void DATA_CreateBuffer( WIFIBATTLEMATCH_SYS *p_wk, HEAPID heapID );
static void DATA_DeleteBuffer( WIFIBATTLEMATCH_SYS *p_wk );

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
typedef enum
{	
	SUBPROCID_CORE,
  SUBPROCID_POKELIST,
  SUBPROCID_BATTLE,
  SUBPROCID_LOGIN,
  SUBPROCID_LISTAFTER,
  SUBPROCID_BTLREC,
  SUBPROCID_LOGOUT,

	SUBPROCID_MAX
} SUBPROC_ID;
static const SUBPROC_DATA sc_subproc_data[SUBPROCID_MAX]	=
{	
	//SUBPROCID_CORE
	{	
		FS_OVERLAY_ID( wifibattlematch_core ),
		&WifiBattleMatchCore_ProcData,
		WBM_CORE_AllocParam,
		WBM_CORE_FreeParam,
	},
  //SUBPROCID_POKELIST,
  { 
	  NO_OVERLAY_ID,
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
		FS_OVERLAY_ID( wifibattlematch_core ),
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
    if( p_param->mode == WIFIBATTLEMATCH_MODE_RANDOM )
    { 
      //�����_���}�b�`�̓|�P�Z��WIFI�J�E���^�[�������A
      //�Q�[���V�X�e�����Ń�������H���Ă���̂ŁAHEAPID_PROC�ɃV�X�e��������
      parentID  = HEAPID_PROC;
    }
    else
    { 
      //����ȊO�́A�^�C�g����ʂ��炭�邽��HEAPID_APP������ɂ���̂ŁA
      //HEAPID_APP������炤
      parentID  = GFL_HEAPID_APP;
    }
  }
  
  NAGI_Printf( "work %d + %d *2\n", sizeof(WIFIBATTLEMATCH_SYS), sizeof(WIFIBATTLEMATCH_ENEMYDATA) );

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( parentID, HEAPID_WIFIBATTLEMATCH_SYS, 0x7000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFIBATTLEMATCH_SYS), HEAPID_WIFIBATTLEMATCH_SYS );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_SYS) );
  GFL_STD_MemCopy( p_param_adrs, &p_wk->param, sizeof(WIFIBATTLEMATCH_PARAM) );
  if( p_wk->param.p_game_data == NULL )
  { 
    p_wk->param.p_game_data = GAMEDATA_Create( GFL_HEAPID_APP );
    p_wk->is_create_gamedata  = TRUE;
  }
  if( p_wk->p_player_btl_party == NULL )
  { 
    p_wk->p_player_btl_party = PokeParty_AllocPartyWork( HEAPID_WIFIBATTLEMATCH_SYS );
  }
  if( p_wk->p_enemy_btl_party == NULL )
  { 
    p_wk->p_enemy_btl_party = PokeParty_AllocPartyWork( HEAPID_WIFIBATTLEMATCH_SYS );
  }

  OS_Printf( "�����_���}�b�`���� �g�p�|�P%d ���[��%d\n", p_wk->param.poke, p_wk->param.btl_rule );

  p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_START;

  //�f�[�^�o�b�t�@�쐬
  DATA_CreateBuffer( p_wk, HEAPID_WIFIBATTLEMATCH_SYS );

  //���W���[���쐬
	SUBPROC_Init( &p_wk->subproc, sc_subproc_data, p_wk, HEAPID_WIFIBATTLEMATCH_SYS );
  SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_LOGIN );

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
	SUBPROC_Exit( &p_wk->subproc );

  //�f�[�^�o�b�t�@�j��
  DATA_DeleteBuffer( p_wk );

  if( p_wk->p_enemy_btl_party )
  { 
    GFL_HEAP_FreeMemory( p_wk->p_enemy_btl_party );
    p_wk->p_enemy_btl_party = NULL;
  }
  if( p_wk->p_player_btl_party )
  { 
    GFL_HEAP_FreeMemory( p_wk->p_player_btl_party );
    p_wk->p_player_btl_party = NULL;
  }

  if( p_wk->is_create_gamedata )
  { 
    GAMEDATA_Delete( p_wk->param.p_game_data );
  }

  //�����_���ΐ�ȊO�̓^�C�g������Ă΂��̂ŁA�߂�̓^�C�g��
  if( p_wk->param.mode != WIFIBATTLEMATCH_MODE_RANDOM )
  { 
    GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);
  }

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
			BOOL is_end;
			is_end	= SUBPROC_Main( &p_wk->subproc );

			if( is_end )
			{	
				*p_seq	= WBM_SYS_SEQ_EXIT;
			}

      //���̃��[�J��PROC����PROC�J�ڂ��邽�߁A�G���[�V�X�e���������œ�����
      //if( p_wk->param.mode == WIFIBATTLEMATCH_MODE_WIFI )
      { 
        const GFL_PROC_MAIN_STATUS  status  = SUBPROC_GetStatus( &p_wk->subproc );
        if( status == GFL_PROC_MAIN_CHANGE || status == GFL_PROC_MAIN_NULL )
        { 
          NetErr_DispCall(FALSE);
        }
      }
		}
		break;

	case WBM_SYS_SEQ_FADEOUT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
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
 *			SUBPROC�V�X�e��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SUBPROC�V�X�e��	������
 *
 *	@param	SUBPROC_WORK *p_wk	���[�N
 *	@param	cp_procdata_tbl			�v���Z�X�ڑ��e�[�u��
 *	@param	void *p_wk_adrs			�A���b�N�֐��Ɖ���֐��ɓn�����[�N
 *	@param	heapID							�V�X�e���\�z�p�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_Init( SUBPROC_WORK *p_wk, const SUBPROC_DATA *cp_procdata_tbl, void *p_wk_adrs, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(SUBPROC_WORK) );
	p_wk->p_procsys				= GFL_PROC_LOCAL_boot( heapID );
	p_wk->p_wk_adrs				= p_wk_adrs;
	p_wk->cp_procdata_tbl	= cp_procdata_tbl;
	p_wk->heapID					= heapID;
}

//----------------------------------------------------------------------------
/**
 *	@brief	SUBPROC�V�X�e��	���C������
 *
 *	@param	SUBPROC_WORK *p_wk	���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��	FALSE�Ȃ��PROC�����݂���
 */
//-----------------------------------------------------------------------------
static BOOL SUBPROC_Main( SUBPROC_WORK *p_wk )
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
		p_wk->now_procID	= p_wk->next_procID;
		p_wk->seq	= SEQ_ALLOC_PARAM;
		break;

	case SEQ_ALLOC_PARAM:
		//�v���Z�X�����쐬�֐�������ΌĂяo��
		if( p_wk->cp_procdata_tbl[ p_wk->now_procID ].alloc_func )
		{	
			p_wk->p_proc_param	= p_wk->cp_procdata_tbl[ p_wk->now_procID ].alloc_func(
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
			if( p_wk->cp_procdata_tbl[	p_wk->now_procID ].free_func( p_wk->p_proc_param, p_wk->p_wk_adrs ) )
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
		if( p_wk->now_procID != p_wk->next_procID )
		{	
			p_wk->seq	= SEQ_INIT;
		}
		else
		{	
			p_wk->seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  SUBPROC�V�X�e�� PROC��Ԃ��擾
 *
 *	@param	const SUBPROC_WORK *cp_wk   ���[�N
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static GFL_PROC_MAIN_STATUS SUBPROC_GetStatus( const SUBPROC_WORK *cp_wk )
{ 
  return cp_wk->status;
}

//----------------------------------------------------------------------------
/**
 *	@brief	SUBPROC�V�X�e��	�j��
 *
 *	@param	SUBPROC_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_Exit( SUBPROC_WORK *p_wk )
{	
	GF_ASSERT( p_wk->p_proc_param == NULL );

	GFL_PROC_LOCAL_Exit( p_wk->p_procsys );
	GFL_STD_MemClear( p_wk, sizeof(SUBPROC_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	SUBPROC�V�X�e��	�v���Z�X���N�G�X�g
 *
 *	@param	SUBPROC_WORK *p_wk	���[�N
 *	@param	proc_id							�Ăԃv���Z�XID
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_CallProc( SUBPROC_WORK *p_wk, u32 procID )
{	
	p_wk->next_procID	= procID;
}

//=============================================================================
/**
 *		�T�u�v���Z�X�p�쐬�j��
 */
//=============================================================================
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
static void *WBM_CORE_AllocParam( HEAPID heapID, void *p_wk_adrs )
{	

  WIFIBATTLEMATCH_CORE_PARAM  *p_param;
  WIFIBATTLEMATCH_SYS         *p_wk     = p_wk_adrs;

  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_CORE_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_CORE_PARAM) );
	p_param->p_param	      = &p_wk->param;
  p_param->mode           = p_wk->core_mode;
  p_param->retmode        = p_wk->core_ret;
  p_param->btl_result     = p_wk->btl_result;
  p_param->p_player_data  = p_wk->p_player_data;
  p_param->p_enemy_data   = p_wk->p_enemy_data;
  p_param->p_svl_result   = &p_wk->svl_result;
  p_param->p_rndmatch     = SaveData_GetRndMatch( GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data ) );

  PMSND_PlayBGM( WBM_SND_SEQ_MAIN );
		
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
static BOOL WBM_CORE_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{	
  WIFIBATTLEMATCH_SYS         *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_param_adrs;

  p_wk->core_ret = p_param->retmode;
  switch( p_param->result )
  { 
  case WIFIBATTLEMATCH_CORE_RESULT_NEXT_BATTLE:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_POKELIST );
    break;
    
  case WIFIBATTLEMATCH_CORE_RESULT_NEXT_REC:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_BTLREC );
    break;
  
  case WIFIBATTLEMATCH_CORE_RESULT_ERR_NEXT_LOGIN:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_LOGIN );
    break;

  case WIFIBATTLEMATCH_CORE_RESULT_FINISH:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_LOGOUT );
    break;
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
static void *POKELIST_AllocParam( HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SUBPROC_PARAM    *p_param;
  WIFIBATTLEMATCH_SYS               *p_wk   = p_wk_adrs;
  int reg_no;

  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_SUBPROC_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_SUBPROC_PARAM) );

  if( p_wk->param.mode == WIFIBATTLEMATCH_MODE_RANDOM )
  { 

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
    }

    p_param->regulation = (REGULATION*)PokeRegulation_LoadDataAlloc( reg_no, heapID );
  }
  else if( p_wk->param.mode == WIFIBATTLEMATCH_MODE_WIFI )
  { 
    SAVE_CONTROL_WORK *p_sv   = GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data );
    p_param->regulation       = SaveData_GetRegulation( p_sv,0 );
  }
  else
  { 
    GF_ASSERT(0);
  }

  p_param->p_party    = p_wk->p_player_btl_party;
  p_param->gameData   = p_wk->param.p_game_data;

  //�����̃f�[�^
  { 
    int i;
    WIFIBATTLEMATCH_ENEMYDATA *p_player;
    POKEPARTY *p_party;
    POKEMON_PARAM *pp;

    p_player = p_wk->p_player_data;
    p_party  = (POKEPARTY*)p_player->pokeparty;
    for( i = 0; i < PokeParty_GetPokeCount( p_party ); i++ )
    {
      pp = PokeParty_GetMemberPointer( p_party, i );
      p_param->ppp[i] = PP_GetPPPPointer( pp );
    }
  }

  //�G�f�[�^
  { 
    WIFIBATTLEMATCH_ENEMYDATA *p_enemy;
    POKEPARTY *p_party;

    p_enemy = p_wk->p_enemy_data;
    p_param->enemyName      = MyStatus_GetMyName( (MYSTATUS*)p_enemy->mystatus );
    p_param->enemyPokeParty = (POKEPARTY*)p_enemy->pokeparty;
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
static BOOL POKELIST_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_SUBPROC_PARAM   *p_param  = p_param_adrs;

  if( p_param->result == WIFIBATTLEMATCH_SUBPROC_RESULT_SUCCESS )
  { 
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_LISTAFTER );
  }
  else if( p_param->result == WIFIBATTLEMATCH_SUBPROC_RESULT_ERROR_NEXT_LOGIN )
  { 
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_LOGIN );
  }
  GFL_HEAP_FreeMemory( p_param->regulation );
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
static void *BATTLE_AllocParam( HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_BATTLELINK_PARAM  *p_param;
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  
  //�f���o�g���ڑ����[�N
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_BATTLELINK_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_BATTLELINK_PARAM) );
  p_param->p_gamedata  = p_wk->param.p_game_data;

  //�f���p�����[�^
  p_param->p_demo_param = GFL_HEAP_AllocMemory( heapID, sizeof(COMM_BTL_DEMO_PARAM) );
	GFL_STD_MemClear( p_param->p_demo_param, sizeof(COMM_BTL_DEMO_PARAM) );

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
  if( p_wk->param.mode == WIFIBATTLEMATCH_MODE_RANDOM )
  { 
    switch( p_wk->param.btl_rule )
    {
    case WIFIBATTLEMATCH_BTLRULE_SINGLE:    ///< �V���O��
      BTL_SETUP_Single_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case WIFIBATTLEMATCH_BTLRULE_DOUBLE:    ///< �_�u��
      BTL_SETUP_Double_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case WIFIBATTLEMATCH_BTLRULE_TRIPLE:    ///< �g���v��
      BTL_SETUP_Triple_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case WIFIBATTLEMATCH_BTLRULE_ROTATE:  ///< ���[�e�[�V����
      BTL_SETUP_Rotation_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;
    case WIFIBATTLEMATCH_BTLRULE_SHOOTER:  ///< �V���[�^�[
      BTL_SETUP_Triple_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      //@todo 
      GF_ASSERT( 0 );

      break;
    }
  }
  //WiFI���̃o�g���ݒ�
  else if( p_wk->param.mode == WIFIBATTLEMATCH_MODE_WIFI )
  { 
    SAVE_CONTROL_WORK *p_sv   = GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data );
    REGULATION* p_reg         = SaveData_GetRegulation( p_sv,0 );

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

//    case REGULATION_BATTLE_SH:    ///< �V���[�^�[
      default:
      //@todo
      BTL_SETUP_Triple_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;
    }
  }
  else
  { 
    GF_ASSERT(0);
  }


  BATTLE_PARAM_SetPokeParty( p_param->p_btl_setup_param, p_wk->p_player_btl_party, BTL_CLIENT_PLAYER ); 

  //�^�揀��
  BTL_SETUP_AllocRecBuffer( p_param->p_btl_setup_param, heapID );

  GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );

  //�^����Ƀo�g������ݒ�
  BattleRec_Init( HEAPID_WIFIBATTLEMATCH_SYS );
  BattleRec_LoadToolModule();
  BattleRec_StoreSetupParam( p_param->p_btl_setup_param );
  BattleRec_UnloadToolModule();

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
static BOOL BATTLE_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_BATTLELINK_PARAM  *p_param  = p_param_adrs;
  BATTLE_SETUP_PARAM  *p_btl_param  = p_param->p_btl_setup_param;
  WIFIBATTLEMATCH_BATTLELINK_RESULT result  = p_param->result;

  //�󂯎��
  p_wk->btl_result  = p_btl_param->result;
  p_wk->btl_rule  = p_btl_param->rule;
  OS_FPrintf( 3, "�o�g������ %d \n", p_wk->btl_result);

  //�j��
  BATTLE_PARAM_Release( p_param->p_btl_setup_param );
	GFL_HEAP_FreeMemory( p_param->p_btl_setup_param );
	GFL_HEAP_FreeMemory( p_param->p_demo_param );
	GFL_HEAP_FreeMemory( p_param );

  //����PROC
  switch( result )
  {
  case WIFIBATTLEMATCH_BATTLELINK_RESULT_SUCCESS:
    p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE;
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );
    break;

  case WIFIBATTLEMATCH_BATTLELINK_RESULT_DISCONNECT:
    //�ؒf���ꂽ
    p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );
    break;
    
  case WIFIBATTLEMATCH_BATTLELINK_RESULT_EVIL:
    p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );
    break;
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
static void *LOGIN_AllocParam( HEAPID heapID, void *p_wk_adrs )
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
static BOOL LOGIN_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  WIFILOGIN_PARAM  *p_param     = p_param_adrs;
  WIFILOGIN_RESULT  result      = p_param->result;

  GFL_HEAP_FreeMemory( p_param );

  switch( result )
  { 
  case WIFILOGIN_RESULT_LOGIN:
    p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_START;
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );
    break;

  case WIFILOGIN_RESULT_CANCEL:
    //PROC���S�Ď��ʂƏI������̂�
    //�Ȃɂ���΂Ȃ��ŏI���
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
static void *WBM_LISTAFTER_AllocParam( HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_LISTAFTER_PARAM *p_param;
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_LISTAFTER_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_LISTAFTER_PARAM) );
  p_param->p_param        = &p_wk->param;
  p_param->p_player_btl_party = p_wk->p_player_btl_party;
  p_param->p_enemy_btl_party  = p_wk->p_enemy_btl_party;

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
static BOOL WBM_LISTAFTER_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS               *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_LISTAFTER_PARAM   *p_param  = p_param_adrs;
  WIFIBATTLEMATCH_LISTAFTER_RESULT  result    = p_param->result;

  GFL_HEAP_FreeMemory( p_param );

  switch( result )
  { 
  case WIFIBATTLEMATCH_LISTAFTER_RESULT_SUCCESS:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_BATTLE );
    break;

  case WIFIBATTLEMATCH_LISTAFTER_RESULT_ERROR_NEXT_LOGIN:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_LOGIN );
    break;
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
static void *WBM_BTLREC_AllocParam( HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  BTL_REC_SEL_PARAM               *p_param;
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(BTL_REC_SEL_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(BTL_REC_SEL_PARAM) );

  p_param->gamedata   = p_wk->param.p_game_data;
  p_param->b_rec        = TRUE;

  if( p_wk->p_enemy_data->btl_server_version != p_wk->p_player_data->btl_server_version )
  { 
    p_param->b_rec        = FALSE;
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
static BOOL WBM_BTLREC_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  BTL_REC_SEL_PARAM               *p_param  = p_param_adrs;

  GFL_HEAP_FreeMemory( p_param );

  //�^��o�b�t�@�I��
  BattleRec_Exit();

  //����PROC
  p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDREC;
  SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );

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
static void *LOGOUT_AllocParam( HEAPID heapID, void *p_wk_adrs )
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
static BOOL LOGOUT_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  WIFILOGOUT_PARAM    *p_param  = p_param_adrs;

  //����PROC�͖���

  GFL_HEAP_FreeMemory( p_param );

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

#if 0
  p_wk->p_gpf_data = GFL_HEAP_AllocMemory( heapID, sizeof( DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA) );
  GFL_STD_MemClear( p_wk->p_gpf_data, sizeof( DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA) );

  p_wk->p_sake_data = GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA) );
  GFL_STD_MemClear( p_wk->p_gpf_data, sizeof(WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA) );
#endif
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
#if 0
  GFL_HEAP_FreeMemory( p_wk->p_sake_data );
  GFL_HEAP_FreeMemory( p_wk->p_gpf_data );
#endif
  GFL_HEAP_FreeMemory( p_wk->p_player_data );
  GFL_HEAP_FreeMemory( p_wk->p_enemy_data );
}
