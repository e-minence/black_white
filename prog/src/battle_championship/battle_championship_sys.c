//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		battle_championship_sys.c
 *	@brief  �o�g�����Ǘ��V�X�e��
 *	@author	Toru=Nagihashi
 *	@data		2010.03.04
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

//�v���Z�X
#include "battle_championship_core.h"
#include "title/title.h"
#include "net_app/wifibattlematch.h"
#include "net_app/btl_rec_sel.h"

//�O�����J
#include "battle_championship/battle_championship.h"

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
//-------------------------------------
///�V�X�e���I�����̍s����	
//=====================================
typedef enum
{
  BC_SYS_NEXT_TITLE,
  BC_SYS_NEXT_WIFICUP,
} BC_SYS_NEXT;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���[�N
//=====================================
typedef struct _BC_SYS_SUBPROC_WORK BC_SYS_SUBPROC_WORK;

//-------------------------------------
///	�V�X�e�����[�N
//=====================================
typedef struct
{ 
  //�v���Z�X�Ǘ��V�X�e��
  BC_SYS_SUBPROC_WORK          *p_subproc;

  //�Q�[���f�[�^�i�^�C�g�����璼�ڗ���̂ő��݂��Ȃ����ߍ쐬�j
  GAMEDATA                      *p_gamedata;

  //���̃V�X�e���̊J�n���[�h
  BATTLE_CHAMPIONSHIP_MODE      mode;

  //�V�X�e���I�����̍s����
  BC_SYS_NEXT                   next;
}BC_SYS_WORK;
//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT BATTLE_CHAMPIONSHIP_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BATTLE_CHAMPIONSHIP_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BATTLE_CHAMPIONSHIP_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	�v���Z�X�p�����쐬�E���
//=====================================
//�`�����s�I���V�b�v�R�A
static void *BC_CORE_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL BC_CORE_FreeParam( void *p_param_adrs, void *p_wk_adrs );

//�^��
static void *BC_BTLREC_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL BC_BTLREC_FreeParam( void *p_param_adrs, void *p_wk_adrs );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �T�u�v���Z�X
 *				    �E�v���Z�X���s��������V�X�e��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�T�u�v���Z�X�������E����֐��R�[���o�b�N
//=====================================
typedef void *(*BC_SYS_SUBPROC_ALLOC_FUNCTION)( HEAPID heapID, void *p_wk_adrs );
typedef BOOL (*BC_SYS_SUBPROC_FREE_FUNCTION)( void *p_param, void *p_wk_adrs );

//-------------------------------------
///	�T�u�v���Z�X�ݒ�\����
//=====================================
typedef struct 
{
	FSOverlayID							    ov_id;
	const GFL_PROC_DATA			    *cp_procdata;
	BC_SYS_SUBPROC_ALLOC_FUNCTION	alloc_func;
	BC_SYS_SUBPROC_FREE_FUNCTION		free_func;
} BC_SYS_SUBPROC_DATA;

//-------------------------------------
///	�p�u���b�N
//=====================================
static BC_SYS_SUBPROC_WORK * BC_SYS_SUBPROC_Init( const BC_SYS_SUBPROC_DATA *cp_procdata_tbl, u32 tbl_len, void *p_wk_adrs, HEAPID heapID );
static void BC_SYS_SUBPROC_Exit( BC_SYS_SUBPROC_WORK *p_wk );
static BOOL BC_SYS_SUBPROC_Main( BC_SYS_SUBPROC_WORK *p_wk );
static GFL_PROC_MAIN_STATUS BC_SYS_SUBPROC_GetStatus( const BC_SYS_SUBPROC_WORK *cp_wk );
static void BC_SYS_SUBPROC_CallProc( BC_SYS_SUBPROC_WORK *p_wk, u32 procID );
static void BC_SYS_SUBPROC_End( BC_SYS_SUBPROC_WORK *p_wk );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	BATTLE_CHAMPIONSHIP_ProcData =
{	
	BATTLE_CHAMPIONSHIP_PROC_Init,
	BATTLE_CHAMPIONSHIP_PROC_Main,
	BATTLE_CHAMPIONSHIP_PROC_Exit,
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
  SUBPROCID_BTLREC,
  /*
  SUBPROCID_POKELIST,
  SUBPROCID_BATTLE,
  SUBPROCID_BTLREC,
*/
	SUBPROCID_MAX
} SUBPROC_ID;
static const BC_SYS_SUBPROC_DATA sc_subproc_data[SUBPROCID_MAX]	=
{
  //SUBPROCID_CORE
  { 
    NO_OVERLAY_ID,
		&BATTLE_CHAMPIONSHIP_CORE_ProcData,
		BC_CORE_AllocParam,
		BC_CORE_FreeParam,
  },

  //SUBPROCID_BTLREC
  { 
    FS_OVERLAY_ID( btl_rec_sel ),
    &BTL_REC_SEL_ProcData,
    BC_BTLREC_AllocParam,
    BC_BTLREC_FreeParam,
  },

};

//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�������	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BATTLE_CHAMPIONSHIP_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  BC_SYS_WORK   *p_wk;

  //�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BATTLE_CHAMPIONSHIP_SYS, 0x10000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BC_SYS_WORK), HEAPID_BATTLE_CHAMPIONSHIP_SYS );
	GFL_STD_MemClear( p_wk, sizeof(BC_SYS_WORK) );
  p_wk->mode  = (BATTLE_CHAMPIONSHIP_MODE)p_param_adrs;

  //�Q�[���f�[�^
  p_wk->p_gamedata  = GAMEDATA_Create( HEAPID_BATTLE_CHAMPIONSHIP_SYS );

  //���W���[���쐬
	p_wk->p_subproc   = BC_SYS_SUBPROC_Init( sc_subproc_data, SUBPROCID_MAX, p_wk, HEAPID_BATTLE_CHAMPIONSHIP_SYS );
  BC_SYS_SUBPROC_CallProc( p_wk->p_subproc, SUBPROCID_CORE );

  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�������	���C���v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BATTLE_CHAMPIONSHIP_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  BC_SYS_WORK *p_wk = p_wk_adrs;

  //���̍s����w��
  switch( p_wk->next )
  { 
  case BC_SYS_NEXT_TITLE:
    GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);
    break;
  case BC_SYS_NEXT_WIFICUP:
    { 
      WIFIBATTLEMATCH_PARAM *p_param;
      p_param = GFL_HEAP_AllocMemory( GFL_HEAPID_APP, sizeof(WIFIBATTLEMATCH_PARAM) );
      GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_PARAM) );
      p_param->mode             = WIFIBATTLEMATCH_MODE_WIFI;
      p_param->is_auto_release  = TRUE;
      GFL_PROC_SysSetNextProc( FS_OVERLAY_ID(wifibattlematch_sys), &WifiBattleMatch_ProcData, p_param );
    }
    break;
  default:
    GF_ASSERT(0); //�s���悪�w�肳��Ă��Ȃ�
  }


  //���W���[���j��
	BC_SYS_SUBPROC_Exit( p_wk->p_subproc );

  //�Q�[���f�[�^
  GAMEDATA_Delete( p_wk->p_gamedata );

  //�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_BATTLE_CHAMPIONSHIP_SYS );


  return GFL_PROC_RES_FINISH;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�������	���C���v���Z�X����
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BATTLE_CHAMPIONSHIP_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  BC_SYS_WORK *p_wk = p_wk_adrs;
  BOOL is_end;

  //�v���Z�X����
  is_end	= BC_SYS_SUBPROC_Main( p_wk->p_subproc );

  if( is_end )
  {	
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *      �v���Z�X�p�����쐬�E���
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���`�����s�I���V�b�v�p�����쐬
 *
 *	@param	HEAPID heapID   �q�[�vID
 *	@param	*p_wk_adrs      WBM_SUBPROC_Init�ŗ^�������[�N�A�h���X
 *
 *	@return �쐬�����p�����[�^
 */
//-----------------------------------------------------------------------------
static void *BC_CORE_AllocParam( HEAPID heapID, void *p_wk_adrs )
{ 
  BC_SYS_WORK *p_wk = p_wk_adrs;
  BATTLE_CHAMPIONSHIP_CORE_PARAM  *p_param;

  p_param = GFL_HEAP_AllocMemory( heapID, sizeof(BATTLE_CHAMPIONSHIP_CORE_PARAM) );
  GFL_STD_MemClear( p_param, sizeof(BATTLE_CHAMPIONSHIP_CORE_PARAM) );
  p_param->p_gamedata = p_wk->p_gamedata;

  switch( (u32)p_wk->mode )
  {
  case (u32)BATTLE_CHAMPIONSHIP_MODE_MAIN_MENU:
    p_param->mode       = BATTLE_CHAMPIONSHIP_CORE_MODE_MAIN_MEMU;
    break;
  case (u32)BATTLE_CHAMPIONSHIP_MODE_WIFI_MENU:
    p_param->mode       = BATTLE_CHAMPIONSHIP_CORE_MODE_WIFI_MENU;
    break;
  }
  return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���`�����s�I���V�b�v�p�����j��
 *
 *	@param	void *p_param   ALLOC�֐��ō쐬��������
 *	@param	*p_wk_adrs      WBM_SUBPROC_Init�ŗ^�������[�N�A�h���X
 *
 *	@return TRUE�ŏI��  FALSE�ő��s
 */
//-----------------------------------------------------------------------------
static BOOL BC_CORE_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  BC_SYS_WORK *p_wk = p_wk_adrs;
  BATTLE_CHAMPIONSHIP_CORE_PARAM  *p_param  = p_param_adrs;
 
  switch( p_param->ret )
  { 
  case BATTLE_CHAMPIONSHIP_CORE_RET_TITLE:   //�^�C�g���֍s��
    p_wk->next  = BC_SYS_NEXT_TITLE;
    BC_SYS_SUBPROC_End( p_wk->p_subproc );
    break;
  case BATTLE_CHAMPIONSHIP_CORE_RET_WIFICUP: //WIFI���֍s��
    p_wk->next  = BC_SYS_NEXT_WIFICUP;
    BC_SYS_SUBPROC_End( p_wk->p_subproc );
    break;
  case BATTLE_CHAMPIONSHIP_CORE_RET_LIVEBTL: //LIVE�p�o�g���֍s��
    break;
  case BATTLE_CHAMPIONSHIP_CORE_RET_LIVEREC: //LIVE�p�^��֍s��
    break;
  }

  GFL_HEAP_FreeMemory( p_param );
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �^��p�����쐬
 *
 *	@param	HEAPID heapID   �q�[�vID
 *	@param	*p_wk_adrs      WBM_SUBPROC_Init�ŗ^�������[�N�A�h���X
 *
 *	@return �쐬�����p�����[�^
 */
//-----------------------------------------------------------------------------
static void *BC_BTLREC_AllocParam( HEAPID heapID, void *p_wk_adrs )
{ 
  BC_SYS_WORK         *p_wk = p_wk_adrs;
  BTL_REC_SEL_PARAM   *p_param;
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(BTL_REC_SEL_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(BTL_REC_SEL_PARAM) );

  p_param->gamedata   = p_wk->p_gamedata;
  p_param->b_rec      = TRUE;

  //if( p_wk->p_enemy_data->btl_server_version != p_wk->p_player_data->btl_server_version )
  { 
    p_param->b_rec        = FALSE;
  }

  return p_param;

}
//----------------------------------------------------------------------------
/**
 *	@brief  �^��p�����j��
 *
 *	@param	void *p_param   ALLOC�֐��ō쐬��������
 *	@param	*p_wk_adrs      WBM_SUBPROC_Init�ŗ^�������[�N�A�h���X
 *
 *	@return TRUE�ŏI��  FALSE�ő��s
 */
//-----------------------------------------------------------------------------
static BOOL BC_BTLREC_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  BC_SYS_WORK       *p_wk = p_wk_adrs;
  BTL_REC_SEL_PARAM *p_param  = p_param_adrs;

  GFL_HEAP_FreeMemory( p_param );

  //�^��o�b�t�@�I��
 // BattleRec_Exit();

  //����PROC
 // p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDREC;
//  WBM_SYS_SUBPROC_CallProc( p_wk->p_subproc, SUBPROCID_CORE );

  return TRUE;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �T�u�v���Z�X
 *				    �E�v���Z�X���s��������V�X�e��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#define BC_SYS_SUBPROC_END  0xFF
//-------------------------------------
///	���[�N
//=====================================
struct _BC_SYS_SUBPROC_WORK
{
	GFL_PROCSYS			  *p_procsys;
	void						  *p_proc_param;

	HEAPID					  heapID;
	void						  *p_wk_adrs;
	const BC_SYS_SUBPROC_DATA			*cp_procdata_tbl;
  u32               tbl_len;

	u8							  next_procID;
	u8							  now_procID;
	u16							  seq;

  GFL_PROC_MAIN_STATUS  status;
} ;
//-------------------------------------
///	�O�����J
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	BC_SYS_SUBPROC�V�X�e��	������
 *
 *	@param	BC_SYS_SUBPROC_WORK *p_wk	���[�N
 *	@param	cp_procdata_tbl			�v���Z�X�ڑ��e�[�u��
 *	@param	void *p_wk_adrs			�A���b�N�֐��Ɖ���֐��ɓn�����[�N
 *	@param	heapID							�V�X�e���\�z�p�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static BC_SYS_SUBPROC_WORK * BC_SYS_SUBPROC_Init( const BC_SYS_SUBPROC_DATA *cp_procdata_tbl, u32 tbl_len, void *p_wk_adrs, HEAPID heapID )
{	
  BC_SYS_SUBPROC_WORK *p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(BC_SYS_SUBPROC_WORK ) );
	GFL_STD_MemClear( p_wk, sizeof(BC_SYS_SUBPROC_WORK) );
	p_wk->p_procsys				= GFL_PROC_LOCAL_boot( heapID );
	p_wk->p_wk_adrs				= p_wk_adrs;
	p_wk->cp_procdata_tbl	= cp_procdata_tbl;
  p_wk->tbl_len         = tbl_len;
	p_wk->heapID					= heapID;

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	BC_SYS_SUBPROC�V�X�e��	�j��
 *
 *	@param	BC_SYS_SUBPROC_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void BC_SYS_SUBPROC_Exit( BC_SYS_SUBPROC_WORK *p_wk )
{	
	GF_ASSERT( p_wk->p_proc_param == NULL );

	GFL_PROC_LOCAL_Exit( p_wk->p_procsys );

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BC_SYS_SUBPROC�V�X�e��	���C������
 *
 *	@param	BC_SYS_SUBPROC_WORK *p_wk	���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��	FALSE�Ȃ��PROC�����݂���
 */
//-----------------------------------------------------------------------------
static BOOL BC_SYS_SUBPROC_Main( BC_SYS_SUBPROC_WORK *p_wk )
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
		if( p_wk->now_procID == p_wk->next_procID )
		{	
      GF_ASSERT( 0 ); //���̃v���Z�X���Z�b�g����Ă��Ȃ�
		}
		else if( p_wk->next_procID == BC_SYS_SUBPROC_END )
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
 *	@brief  BC_SYS_SUBPROC�V�X�e�� PROC��Ԃ��擾
 *
 *	@param	const BC_SYS_SUBPROC_WORK *cp_wk   ���[�N
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static GFL_PROC_MAIN_STATUS BC_SYS_SUBPROC_GetStatus( const BC_SYS_SUBPROC_WORK *cp_wk )
{ 
  return cp_wk->status;
}

//----------------------------------------------------------------------------
/**
 *	@brief	BC_SYS_SUBPROC�V�X�e��	�v���Z�X���N�G�X�g
 *
 *	@param	BC_SYS_SUBPROC_WORK *p_wk	���[�N
 *	@param	proc_id							�Ăԃv���Z�XID
 *
 */
//-----------------------------------------------------------------------------
static void BC_SYS_SUBPROC_CallProc( BC_SYS_SUBPROC_WORK *p_wk, u32 procID )
{	
	p_wk->next_procID	= procID;
}

//----------------------------------------------------------------------------
/**
 *	@brief	BC_SYS_SUBPROC�V�X�e��	�I��
 *
 *	@param	BC_SYS_SUBPROC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BC_SYS_SUBPROC_End( BC_SYS_SUBPROC_WORK *p_wk )
{ 
  p_wk->next_procID = BC_SYS_SUBPROC_END;
}
