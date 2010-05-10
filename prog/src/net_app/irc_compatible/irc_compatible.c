//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_aura_main.c
 *	@brief	�ԊO���~�j�Q�[��
 *	@author	Toru=Nagihashi
 *	@data		2009.05.11
 *
 *	���̃v���Z�X�́A�e�A�v���P�[�V�����v���Z�X���q��������
 *	�A�v���P�[�V�����Ԃ̏��̂��Ƃ�����邽�߂ɑ��݂���B
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	lib
#include <gflib.h>

//	system
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"
#include "sound/pm_sndsys.h"

//	module
#include "net_app/compatible_irc_sys.h"

//	proc
#include "net_app/irc_menu.h"
#include "net_app/irc_aura.h"
#include "net_app/irc_rhythm.h"
#include "net_app/irc_result.h"
#include "net_app/irc_ranking.h" 

//	mine
#include "net_app/irc_compatible.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef PM_DEBUG
#if defined( DEBUG_ONLY_FOR_toru_nagihashi)
//#define DEBUG_RETURN_TO_PROG	//�߂�{�^���������Ǝ��֐i��
#endif	//DEBUG_ONLY_FOR_toru_nagihashi
#endif	//PM_DEBUG

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�T�u�v���b�N�ړ�
//=====================================
typedef struct {
	GFL_PROCSYS			*p_procsys;
	u32							seq;
	void						*p_proc_param;
} SUBPROC_WORK;

//-------------------------------------
///	�����f�f���C�����[�N
//=====================================
typedef struct _IRC_COMPATIBLE_MAIN_WORK IRC_COMPATIBLE_MAIN_WORK;
typedef void (*SEQ_FUNCTION)( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq );
struct _IRC_COMPATIBLE_MAIN_WORK
{
	//�l�b�g���W���[��
	COMPATIBLE_IRC_SYS	*p_irc;

	//�T�u�v���Z�X���W���[��
	SUBPROC_WORK				subproc;

	//�����f�f�p�����[�^
	IRC_COMPATIBLE_PARAM	*p_param;

	//�V�[�P���X�Ǘ�
	SEQ_FUNCTION				seq_function;
	u16									seq;
	u16									dummy;
	BOOL								is_end;


	//PROC�ԃf�[�^
	IRCMENU_SELECT			select;	//���j���[�őI�񂾂���
	IRCAURA_RESULT			aura_result;
	IRCRHYTHM_RESULT		rhythm_result;
	u8									rhythm_score;
	u8									rhythm_cnt_diff;
	u8									aura_score;
  u8                  aura_minus;
	BOOL								is_init;
	BOOL								is_ranking_ret;
	COMPATIBLE_STATUS	  *p_you_status;

};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//proc
static GFL_PROC_RESULT IRC_COMPATIBLE_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_COMPATIBLE_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_COMPATIBLE_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
//SEQ
static void SEQ_Change( IRC_COMPATIBLE_MAIN_WORK *p_wk, SEQ_FUNCTION seq_function );
static void SEQ_End( IRC_COMPATIBLE_MAIN_WORK *p_wk );
//SEQ_FUNC
static void SEQFUNC_Start( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_End( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_MenuProc( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_CompatibleProc( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_RankingProc( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_DebugCompatibleProc( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq );
//PROCMODE
static void SUBPROC_Init( SUBPROC_WORK *p_wk, HEAPID heapID );
static GFL_PROC_MAIN_STATUS SUBPROC_Main( SUBPROC_WORK *p_wk );
static void SUBPROC_Exit( SUBPROC_WORK *p_wk );
static BOOL SUBPROC_CallProcReq( SUBPROC_WORK *p_wk, u32 proc_id, HEAPID heapID, void *p_wk_adrs );
//PROCCHANE
typedef void *(*SUBPROC_ALLOC_FUNCTION)( HEAPID heapID, void *p_wk_adrs );
typedef void (*SUBPROC_FREE_FUNCTION)( void *p_param, void *p_wk_adrs );
static void *SUBPROC_ALLOC_Menu( HEAPID heapID, void *p_wk_adrs );
static void SUBPROC_FREE_Menu( void *p_param_adrs, void *p_wk_adrs );
static void *SUBPROC_ALLOC_Aura( HEAPID heapID, void *p_wk_adrs );
static void SUBPROC_FREE_Aura( void *p_param_adrs, void *p_wk_adrs );
static void *SUBPROC_ALLOC_Rhythm( HEAPID heapID, void *p_wk_adrs );
static void SUBPROC_FREE_Rhythm( void *p_param_adrs, void *p_wk_adrs );
static void *SUBPROC_ALLOC_Result( HEAPID heapID, void *p_wk_adrs );
static void SUBPROC_FREE_Result( void *p_param_adrs, void *p_wk_adrs );
static void *SUBPROC_ALLOC_Ranking( HEAPID heapID, void *p_wk_adrs );
static void SUBPROC_FREE_Ranking( void *p_param_adrs, void *p_wk_adrs );
//RULE
static u32 RULE_CalcScore( u32 rhythm_score, u32 aura_score, u32 rhythm_minus, u32 aura_minus, const COMPATIBLE_STATUS *cp_my_status, const COMPATIBLE_STATUS *cp_you_status, BOOL is_init, HEAPID heapID );
static u32 RULE_CalcNameScore( const STRCODE	*cp_player1_name, const STRCODE	*cp_player2_name );
static u32 MATH_GetMostOnebit( u32 x, u8 bit );
static u32 RULE_CalcBioRhythm( const COMPATIBLE_STATUS *cp_status );
static u32 RULE_CalcRhythmMinus( u32 cnt_diff );
//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�ԊO���~�j�Q�[�����j���[�p�v���b�N�f�[�^
//=====================================
const GFL_PROC_DATA IrcCompatible_ProcData	= 
{	
	IRC_COMPATIBLE_PROC_Init,
	IRC_COMPATIBLE_PROC_Main,
	IRC_COMPATIBLE_PROC_Exit,
};
//-------------------------------------
///	PROC�Ăяo��
//=====================================
FS_EXTERN_OVERLAY(irc_menu);
FS_EXTERN_OVERLAY(irc_aura);
FS_EXTERN_OVERLAY(irc_rhythm);
FS_EXTERN_OVERLAY(irc_result);
FS_EXTERN_OVERLAY(irc_ranking);

//-------------------------------------
///	PROC�ړ��f�[�^
//=====================================
typedef enum
{	
	SUBPROCID_MENU,
	SUBPROCID_AURA,
	SUBPROCID_RHYTHM,
	SUBPROCID_RESULT,
	SUBPROCID_RANKING,

	SUBPROCID_MAX,
	SUBPROCID_NULL	= SUBPROCID_MAX,
} SUBPROC_ID;
static const struct
{	
	FSOverlayID		ov_id;
	const GFL_PROC_DATA	*cp_procdata;
	SUBPROC_ALLOC_FUNCTION	alloc_func;
	SUBPROC_FREE_FUNCTION		free_func;
} sc_proc_data_tbl[SUBPROCID_MAX]	=
{	
	{	
		FS_OVERLAY_ID(irc_menu),
		&IrcMenu_ProcData,
		SUBPROC_ALLOC_Menu,
		SUBPROC_FREE_Menu,
	},
	{	
		FS_OVERLAY_ID(irc_aura),
		&IrcAura_ProcData,
		SUBPROC_ALLOC_Aura,
		SUBPROC_FREE_Aura,
	},
	{	
		FS_OVERLAY_ID(irc_rhythm),
		&IrcRhythm_ProcData,
		SUBPROC_ALLOC_Rhythm,
		SUBPROC_FREE_Rhythm,
	},
	{	
		FS_OVERLAY_ID(irc_result),
		&IrcResult_ProcData,
		SUBPROC_ALLOC_Result,
		SUBPROC_FREE_Result,
	},
	{	
		FS_OVERLAY_ID(irc_ranking),
		&IrcRanking_ProcData,
		SUBPROC_ALLOC_Ranking,
		SUBPROC_FREE_Ranking,
	}
};

//=============================================================================
/**
 *			PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO���~�j�Q�[�����j���[	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_COMPATIBLE_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_COMPATIBLE_MAIN_WORK	*p_wk;

//�f�o�b�O����NULL�̂Ƃ��ł�����
//�i�f�o�b�O�����GAMESYS_WORK��n���Ȃ����߁j

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCCOMPATIBLE_SYSTEM, 0x08000 );
	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(IRC_COMPATIBLE_MAIN_WORK), HEAPID_IRCCOMPATIBLE_SYSTEM );
	GFL_STD_MemClear( p_wk, sizeof(IRC_COMPATIBLE_MAIN_WORK) );
	p_wk->p_param	= p_param;

	//�ʐM����f�[�^�p�o�b�t�@�쐬
	p_wk->p_you_status	= GFL_HEAP_AllocMemory( HEAPID_IRCCOMPATIBLE_SYSTEM, sizeof(COMPATIBLE_STATUS) );
	GFL_STD_MemClear( p_wk->p_you_status, sizeof(COMPATIBLE_STATUS) );

	//���W���[���쐬
	SUBPROC_Init( &p_wk->subproc, HEAPID_IRCCOMPATIBLE_SYSTEM );

	//0xFFFFFFFF�͉��}����
	//����ڑ����Ȃ��������ɒ����B���̍ہA�ŏ��̐ڑ����Ƀ}�b�N�A�h���X��Ⴂ���̐l�Ƃ���
	//�q����Ȃ��悤�ȏ����ɂ���
	//���d�l�ɂȂ�܂����B090709
	{	
		BOOL is_debug		= FALSE;

#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
		is_debug	= p_wk->p_param->is_only_play;
#endif

    {
      GAMEDATA  *p_gamedata = NULL;

      if( p_wk->p_param->p_gamesys )
      { 
        p_gamedata  = GAMESYSTEM_GetGameData( p_wk->p_param->p_gamesys );
      }
      p_wk->p_irc	= COMPATIBLE_IRC_CreateSystem( 0xFFFFFFFF, p_gamedata, HEAPID_IRCCOMPATIBLE_SYSTEM, is_debug );
    }
	}

	p_wk->is_init	= TRUE;

	{	
		SEQ_Change( p_wk, SEQFUNC_Start );
	}

  GF_ASSERT_MSG( COMPATIBLE_MYSTATUS_SIZE == MyStatus_GetWorkSize(), "number %d != mystatus %d!!\n", COMPATIBLE_MYSTATUS_SIZE, MyStatus_GetWorkSize() );

  PMSND_PauseBGM( TRUE );
  PMSND_PushBGM( );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO���~�j�Q�[�����j���[	���C���v���Z�X�j������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_COMPATIBLE_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_COMPATIBLE_MAIN_WORK	*p_wk;

	p_wk	= p_work;


  PMSND_PopBGM( );
  PMSND_PauseBGM( FALSE );

	//���W���[���j��
	COMPATIBLE_IRC_DeleteSystem( p_wk->p_irc );
	SUBPROC_Exit( &p_wk->subproc );
	
	//�o�b�t�@�j��
	GFL_HEAP_FreeMemory( p_wk->p_you_status );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );
	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_IRCCOMPATIBLE_SYSTEM );
	

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO���~�j�Q�[�����j���[	���C���v���Z�X���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_COMPATIBLE_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
  enum
  { 
    SEQ_BGM_WAIT,
    SEQ_MAIN,
  };

	IRC_COMPATIBLE_MAIN_WORK	*p_wk;
	p_wk	= p_work;

  switch( *p_seq )
  { 
  case SEQ_BGM_WAIT:
    if( !PMSND_CheckFadeOnBGM() )
    { 
      (*p_seq)++;
    }
    break;

  case SEQ_MAIN:
    { 
      GFL_PROC_MAIN_STATUS ret = SUBPROC_Main( &p_wk->subproc );

      if( ret == GFL_PROC_MAIN_NULL )
      {	
        p_wk->seq_function( p_wk, &p_wk->seq );
      }

      if( p_wk->is_end )
      {
        return GFL_PROC_RES_FINISH;
      }
      break;
    }
  }

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
 *				SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ�`�F���W
 *
 *	@param	IRC_COMPATIBLE_MAIN_WORK *p_wk	���[�N
 *	@param	SEQ_FUNCTION										SEQ�֐�
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Change( IRC_COMPATIBLE_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PROC�I��
 *
 *	@param	IRC_COMPATIBLE_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( IRC_COMPATIBLE_MAIN_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}
//=============================================================================
/**
 *				SEQFUNC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�J�n�V�[�P���X
 *
 *	@param	IRC_COMPATIBLE_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Start( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum
	{	
		SEQ_CHANGE_MENU,
	};

	switch( *p_seq )
	{	
	case SEQ_CHANGE_MENU:
		SEQ_Change( p_wk, SEQFUNC_MenuProc );
		break;
	};
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I���V�[�P���X
 *
 *	@param	IRC_COMPATIBLE_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum
	{	
		SEQ_NET_EXIT,
		SEQ_END,
	};

	switch( *p_seq )
	{	
	case SEQ_NET_EXIT:
		//if( COMPATIBLE_IRC_ExitWait( p_wk->p_irc ) )
		{	
			*p_seq	= SEQ_END;
		}
		break;
		
	case SEQ_END:
		SEQ_End( p_wk );
		break;
	};
}

//----------------------------------------------------------------------------
/**
 *	@brief	���j���[�V�[�P���X
 *
 *	@param	IRC_COMPATIBLE_MAIN_WORK *p_wk
 *	@param	*p_seq 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_MenuProc( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum
	{	
		SEQ_PROC_MENU,
		SEQ_SELECT_MENU,
	};

	switch( *p_seq )
	{	
	case SEQ_PROC_MENU:
		if( SUBPROC_CallProcReq( &p_wk->subproc, SUBPROCID_MENU, HEAPID_IRCCOMPATIBLE_SYSTEM, p_wk ) )
		{	
			*p_seq	= SEQ_SELECT_MENU;
		}
		break;

	case SEQ_SELECT_MENU:
		switch( p_wk->select )
		{	
		case IRCMENU_SELECT_COMPATIBLE:
			SEQ_Change( p_wk, SEQFUNC_CompatibleProc );
			break;
		case IRCMENU_SELECT_RANKING:
			SEQ_Change( p_wk, SEQFUNC_RankingProc );
			break;
		case IRCMENU_SELECT_RETURN:
			SEQ_Change( p_wk, SEQFUNC_End );
			break;
		default:
			GF_ASSERT(0);
		}
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����f�f�Q�[���V�[�P���X
 *
 *	@param	IRC_COMPATIBLE_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_CompatibleProc( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_PROC_RHYTHM,
		SEQ_PROC_RHYTHM_RESULT,
		SEQ_PROC_AURA,
		SEQ_PROC_AURA_RESULT,
		SEQ_PROC_RESULT,
		SEQ_CHANGE_MENU,
	};

	switch( *p_seq )
	{	
	case SEQ_INIT:
		p_wk->aura_score		= 0;
		p_wk->rhythm_score	= 0;
		*p_seq	= SEQ_PROC_RHYTHM;
		break;

	case SEQ_PROC_RHYTHM:
		if( SUBPROC_CallProcReq( &p_wk->subproc, SUBPROCID_RHYTHM, HEAPID_IRCCOMPATIBLE_SYSTEM, p_wk ) )
		{	
			*p_seq	= SEQ_PROC_RHYTHM_RESULT;
		}
		break;

	case SEQ_PROC_RHYTHM_RESULT:
#ifdef DEBUG_RETURN_TO_PROG
		if(1)
#else
		if( p_wk->rhythm_result == IRCRHYTHM_RESULT_CLEAR )
#endif	//DEBUG_RETURN_TO_PROG
		{	
			*p_seq	= SEQ_PROC_AURA;
		}
		else if( p_wk->rhythm_result == IRCRHYTHM_RESULT_RESULT )
		{	
			p_wk->aura_score		= 0;
			p_wk->rhythm_score	= 0;
			*p_seq	= SEQ_PROC_RESULT;
		}
		else
		{	
			*p_seq	= SEQ_CHANGE_MENU;
		}
		break;

	case SEQ_PROC_AURA:
		if( SUBPROC_CallProcReq( &p_wk->subproc, SUBPROCID_AURA, HEAPID_IRCCOMPATIBLE_SYSTEM, p_wk ) )
		{	
			*p_seq	= SEQ_PROC_AURA_RESULT;
		}
		break;

	case SEQ_PROC_AURA_RESULT:
#ifdef DEBUG_RETURN_TO_PROG
		if(1)
#else
		if( p_wk->aura_result == IRCAURA_RESULT_CLEAR )
#endif //DEBUG_RETURN_TO_PROG
		{	
			*p_seq	= SEQ_PROC_RESULT;
		}
		else if( p_wk->aura_result == IRCAURA_RESULT_RESULT )
		{	
			p_wk->aura_score		= 0;
			p_wk->rhythm_score	= 0;
			*p_seq	= SEQ_PROC_RESULT;
		}
		else
		{	
			*p_seq	= SEQ_CHANGE_MENU;
		}
		break;

	case SEQ_PROC_RESULT:
		if( SUBPROC_CallProcReq( &p_wk->subproc, SUBPROCID_RESULT, HEAPID_IRCCOMPATIBLE_SYSTEM, p_wk ) )
		{	
			*p_seq	= SEQ_CHANGE_MENU;
		}
		break;

	case SEQ_CHANGE_MENU:
		SEQ_Change( p_wk, SEQFUNC_MenuProc );
		break;
	};
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����L���OPROC
 *
 *	@param	IRC_COMPATIBLE_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_RankingProc( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq )
{	enum
	{	
		SEQ_INIT,
		SEQ_PROC_RANKING,
		SEQ_CHANGE_MENU,
	};

	switch( *p_seq )
	{	
	case SEQ_INIT:
		*p_seq	= SEQ_PROC_RANKING;
		break;

	case SEQ_PROC_RANKING:
		if( SUBPROC_CallProcReq( &p_wk->subproc, SUBPROCID_RANKING, HEAPID_IRCCOMPATIBLE_SYSTEM, p_wk ) )
		{	
			*p_seq	= SEQ_CHANGE_MENU;
		}
		break;

	case SEQ_CHANGE_MENU:
		SEQ_Change( p_wk, SEQFUNC_MenuProc );
		break;
	};
}

#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O����PROC�Q�[��
 *
 *	@param	IRC_COMPATIBLE_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_DebugCompatibleProc( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_PROC_RHYTHM,
		SEQ_PROC_AURA,
		SEQ_PROC_RESULT,
		SEQ_END,
	};

	switch( *p_seq )
	{	
	case SEQ_INIT:
		p_wk->aura_score		= 0;
		p_wk->rhythm_score	= 0;
		*p_seq	= SEQ_PROC_RHYTHM;
		break;

	case SEQ_PROC_RHYTHM:
		if( SUBPROC_CallProcReq( &p_wk->subproc, SUBPROCID_RHYTHM, HEAPID_IRCCOMPATIBLE_SYSTEM, p_wk ) )
		{	
			*p_seq	= SEQ_PROC_AURA;
		}
		break;

	case SEQ_PROC_AURA:
		if( SUBPROC_CallProcReq( &p_wk->subproc, SUBPROCID_AURA, HEAPID_IRCCOMPATIBLE_SYSTEM, p_wk ) )
		{	
			*p_seq	= SEQ_PROC_RESULT;
		}
		break;

	case SEQ_PROC_RESULT:
		if( SUBPROC_CallProcReq( &p_wk->subproc, SUBPROCID_RESULT, HEAPID_IRCCOMPATIBLE_SYSTEM, p_wk ) )
		{	
			*p_seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		SEQ_End( p_wk );
		break;
	};
}
#endif //DEBUG_IRC_COMPATIBLE_ONLYPLAY
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
 *	@param	heapID							�V�X�e���\�z�p�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_Init( SUBPROC_WORK *p_wk, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(SUBPROC_WORK) );
	p_wk->p_procsys	= GFL_PROC_LOCAL_boot( heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	SUBPROC�V�X�e��	���C������
 *
 *	@param	SUBPROC_WORK *p_wk	���[�N
 *
 *	@retval	GFL_PROC_MAIN_STATUS
 */
//-----------------------------------------------------------------------------
static GFL_PROC_MAIN_STATUS SUBPROC_Main( SUBPROC_WORK *p_wk )
{	
	return GFL_PROC_LOCAL_Main( p_wk->p_procsys );
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
	GFL_PROC_LOCAL_Exit( p_wk->p_procsys );
	GFL_STD_MemClear( p_wk, sizeof(SUBPROC_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	SUBPROC�V�X�e��	�v���Z�X���N�G�X�g
 *
 *	@param	SUBPROC_WORK *p_wk	���[�N
 *	@param	proc_id							�Ăԃv���Z�XID
 *	@param	heapID							�v���Z�X�ւ̈������쐬�p�q�[�vID
 *	@param	*p_wk_adrs					�v���Z�X�ւ̈����쐬�p�ɓn�����
 *
 *	@retval	TRUE	�v���Z�X���I������
 *	@retval	FALSE	�܂��v���Z�X���I�����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL SUBPROC_CallProcReq( SUBPROC_WORK *p_wk, u32 proc_id, HEAPID heapID, void *p_wk_adrs )
{	
	enum
	{	
		SEQ_ALLOC_PARAM,
		SEQ_CALL_PROC,
		SEQ_FREE_PARAM,
		SEQ_END,
	};

	switch( p_wk->seq )
	{	
	case SEQ_ALLOC_PARAM:
		if( sc_proc_data_tbl[ proc_id ].alloc_func )
		{	
			p_wk->p_proc_param	= sc_proc_data_tbl[ proc_id ].alloc_func( heapID, p_wk_adrs );
		}
		else
		{	
			p_wk->p_proc_param	= NULL;
		}
		p_wk->seq	= SEQ_CALL_PROC;
		break;

	case SEQ_CALL_PROC:
		GFL_PROC_LOCAL_CallProc( p_wk->p_procsys, sc_proc_data_tbl[	proc_id ].ov_id,
				sc_proc_data_tbl[	proc_id ].cp_procdata, p_wk->p_proc_param );
		p_wk->seq	= SEQ_FREE_PARAM;
		break;

	case SEQ_FREE_PARAM:
		if( sc_proc_data_tbl[	proc_id ].free_func )
		{	
			sc_proc_data_tbl[	proc_id ].free_func( p_wk->p_proc_param, p_wk_adrs );
		}
		p_wk->seq	= SEQ_END;
		break;

	case SEQ_END:
		p_wk->seq	= 0;
		return TRUE;
	}

	return FALSE;
}
//=============================================================================
/**
 *				PROC�ړ��p�p�����[�^�쐬
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[�v���b�N�p�p�����[�^�쐬
 *
 *	@param	heapID													�q�[�vID
 *	@param	p_wk_adrs												���[�N�A�h���X
 *
 *	@return	�I�[���`�F�b�N�ɓn���p�����[�^
 */
//-----------------------------------------------------------------------------
static void *SUBPROC_ALLOC_Menu( HEAPID heapID, void *p_wk_adrs )
{	
	IRC_COMPATIBLE_MAIN_WORK *p_wk;
	IRC_MENU_PARAM	*p_param;

	p_wk	= p_wk_adrs;

	p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(IRC_MENU_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(IRC_MENU_PARAM));
	p_param->p_gamesys	= p_wk->p_param->p_gamesys;
	p_param->p_irc			= p_wk->p_irc;
	p_param->p_you_status	= p_wk->p_you_status;

	if( p_wk->is_init == TRUE )
	{	
		p_param->mode				= IRCMENU_MODE_INIT;
		p_wk->is_init				= FALSE;
	}
	else
	{	
		if( p_wk->is_ranking_ret )
		{	
			p_wk->is_ranking_ret	= FALSE;
			p_param->mode				= IRCMENU_MODE_RANKING_RETURN;
		}
		else
		{	
			p_param->mode				= IRCMENU_MODE_RETURN;
		}
	}
	
	return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[�v���b�N�p�p�����[�^�j��
 *
 *	@param	p_param													�p�����[�^�A�h���X
 *	@param	p_wk_adrs												���[�N�A�h���X
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_FREE_Menu( void *p_param_adrs, void *p_wk_adrs )
{	
	IRC_COMPATIBLE_MAIN_WORK *p_wk;
	IRC_MENU_PARAM	*p_param;
	
	p_wk		= p_wk_adrs;
	p_param	= p_param_adrs;


	p_wk->select	= p_param->select;

	GFL_HEAP_FreeMemory( p_param );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[���`�F�b�N�v���b�N�p�p�����[�^�쐬
 *
 *	@param	heapID													�q�[�vID
 *	@param	p_wk_adrs												���[�N�A�h���X
 *
 *	@return	�I�[���`�F�b�N�ɓn���p�����[�^
 */
//-----------------------------------------------------------------------------
static void *SUBPROC_ALLOC_Aura( HEAPID heapID, void *p_wk_adrs )
{	
	IRC_COMPATIBLE_MAIN_WORK *p_wk;
	IRC_AURA_PARAM	*p_param;

	p_wk	= p_wk_adrs;

	p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(IRC_AURA_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(IRC_AURA_PARAM)) ;
	p_param->p_gamesys	= p_wk->p_param->p_gamesys;
	p_param->p_irc			= p_wk->p_irc;
	p_param->p_you_status	= p_wk->p_you_status;

#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
	p_param->is_only_play	= p_wk->p_param->is_only_play;
#endif //DEBUG_IRC_COMPATIBLE_ONLYPLAY

	return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[���`�F�b�N�v���b�N�p�p�����[�^�j��
 *
 *	@param	p_param													�p�����[�^�A�h���X
 *	@param	p_wk_adrs												���[�N�A�h���X
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_FREE_Aura( void *p_param_adrs, void *p_wk_adrs )
{	
	IRC_COMPATIBLE_MAIN_WORK *p_wk;
	IRC_AURA_PARAM	*p_param;
	
	p_wk		= p_wk_adrs;
	p_param	= p_param_adrs;

	p_wk->aura_result	= p_param->result;
	p_wk->aura_score	= p_param->score;
  p_wk->aura_minus  = p_param->minus;

	GFL_HEAP_FreeMemory( p_param );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���Y���`�F�b�N�v���b�N�p�p�����[�^�쐬
 *
 *	@param	heapID													�q�[�vID
 *	@param	p_wk_adrs												���[�N�A�h���X
 *
 *	@return	���Y���`�F�b�N�ɓn���p�����[�^
 */
//-----------------------------------------------------------------------------
static void *SUBPROC_ALLOC_Rhythm( HEAPID heapID, void *p_wk_adrs )
{	
	IRC_COMPATIBLE_MAIN_WORK *p_wk;
	IRC_RHYTHM_PARAM	*p_param;

	p_wk	= p_wk_adrs;

	p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(IRC_RHYTHM_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(IRC_RHYTHM_PARAM)) ;
	p_param->p_gamesys	= p_wk->p_param->p_gamesys;
	p_param->p_irc			= p_wk->p_irc;
	p_param->p_you_status	= p_wk->p_you_status;

#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
	p_param->is_only_play	= p_wk->p_param->is_only_play;
#endif //DEBUG_IRC_COMPATIBLE_ONLYPLAY

	return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���Y���`�F�b�N�v���b�N�p�p�����[�^�j��
 *
 *	@param	p_param													�p�����[�^�A�h���X
 *	@param	p_wk_adrs												���[�N�A�h���X
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_FREE_Rhythm( void *p_param_adrs, void *p_wk_adrs )
{	
	IRC_COMPATIBLE_MAIN_WORK *p_wk;
	IRC_RHYTHM_PARAM	*p_param;
	
	p_wk		= p_wk_adrs;
	p_param	= p_param_adrs;

	p_wk->rhythm_result	= p_param->result;
	p_wk->rhythm_score	= p_param->score;
  p_wk->rhythm_cnt_diff  = p_param->cnt_diff;

	GFL_HEAP_FreeMemory( p_param );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���ʐf�f�v���b�N�p�p�����[�^�쐬
 *
 *	@param	heapID													�q�[�vID
 *	@param	p_wk_adrs												���[�N�A�h���X
 *
 *	@return	���Y���`�F�b�N�ɓn���p�����[�^
 */
//-----------------------------------------------------------------------------
static void *SUBPROC_ALLOC_Result( HEAPID heapID, void *p_wk_adrs )
{	
	IRC_COMPATIBLE_MAIN_WORK *p_wk;
	IRC_RESULT_PARAM	*p_param;

	p_wk	= p_wk_adrs;

	p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(IRC_RESULT_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(IRC_RESULT_PARAM)) ;
	p_param->p_gamesys	= p_wk->p_param->p_gamesys;
	p_param->p_irc			= p_wk->p_irc;
	p_param->p_you_status	= p_wk->p_you_status;

#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
	if( p_wk->p_param->is_only_play )
	{	
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
    { 
      p_param->score	= 100;
    }
    else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
    { 
      p_param->score	= 90;
    }
    else
    { 
      p_param->score	= 80;
    }
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_A )
    { 
      p_param->score	-= 1;
    }
		p_param->is_only_play	= p_wk->p_param->is_only_play;
		return p_param;
	}
#endif



	//���_�v�Z
	{	
		if( p_wk->rhythm_score != 0 && p_wk->aura_score != 0 )
		{	
      BOOL is_init;
      u32 rhythm_minus;
      COMPATIBLE_STATUS my_status;

      { 
        MYSTATUS  *p_youstatus  = (MYSTATUS*)p_wk->p_you_status->my_status;

        GAMEDATA  *p_gamedata  = GAMESYSTEM_GetGameData( p_wk->p_param->p_gamesys );
        SAVE_CONTROL_WORK *p_sv_ctrl;
        IRC_COMPATIBLE_SAVEDATA *p_sv;

#ifdef PM_DEBUG
        if( p_wk->p_param->p_gamesys == NULL )
        { 
          p_sv_ctrl = SaveControl_GetPointer();
        }
        else
#endif 
        { 
          p_sv_ctrl = GAMEDATA_GetSaveControlWork( GAMESYSTEM_GetGameData( p_wk->p_param->p_gamesys ) );
        }
        //�t�B�[�����O�`�F�b�N�Z�[�u�f�[�^�擾
        p_sv	= IRC_COMPATIBLE_SV_GetSavedata( p_sv_ctrl );


        is_init  = !IRC_COMPATIBLE_SV_IsPlayed( p_sv,
            MyStatus_GetID(p_youstatus) );
      }

      COMPATIBLE_IRC_GetStatus( p_wk->p_param->p_gamesys, &my_status );

      //���Y���̃}�C�i�X�_���v�Z
      rhythm_minus  = RULE_CalcRhythmMinus( p_wk->rhythm_cnt_diff );

      //���_�v�Z
			p_param->score			= RULE_CalcScore( p_wk->rhythm_score, p_wk->aura_score,
          rhythm_minus, p_wk->aura_minus, &my_status, p_wk->p_you_status, is_init,
          HEAPID_IRCCOMPATIBLE_SYSTEM );
		}
	}

	return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���ʐf�f�v���b�N�p�p�����[�^�j��
 *
 *	@param	p_param													�p�����[�^�A�h���X
 *	@param	p_wk_adrs												���[�N�A�h���X
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_FREE_Result( void *p_param_adrs, void *p_wk_adrs )
{	
	IRC_COMPATIBLE_MAIN_WORK *p_wk;
	IRC_RESULT_PARAM	*p_param;
	
	p_wk		= p_wk_adrs;
	p_param	= p_param_adrs;

	GFL_HEAP_FreeMemory( p_param );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����L���O�p�v���b�N�p�p�����[�^�쐬
 *
 *	@param	void *p_param_adrs	�p�����[�^�A�h���X
 *	@param	*p_wk_adrs					���[�N�A�h���X
 *
 */
//-----------------------------------------------------------------------------
static void *SUBPROC_ALLOC_Ranking( HEAPID heapID, void *p_wk_adrs )
{	
	IRC_COMPATIBLE_MAIN_WORK *p_wk;
	IRC_RANKING_PARAM	*p_param;

	p_wk	= p_wk_adrs;

	p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(IRC_RANKING_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(IRC_RANKING_PARAM)) ;
	p_param->p_gamesys	= p_wk->p_param->p_gamesys;
	return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����L���O�p�v���b�N�p�p�����[�^�j��
 *
 *	@param	void *p_param_adrs	�p�����[�^�A�h���X
 *	@param	*p_wk_adrs					���[�N�A�h���X
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_FREE_Ranking( void *p_param_adrs, void *p_wk_adrs )
{	
	IRC_COMPATIBLE_MAIN_WORK *p_wk;
	IRC_RANKING_PARAM	*p_param;
	
	p_wk		= p_wk_adrs;
	p_param	= p_param_adrs;

	//���
	GFL_HEAP_FreeMemory( p_param );

	//�����L���O����߂������Ƃ�ʒm
	p_wk->is_ranking_ret	= TRUE;
}
//=============================================================================
/**
 *				RULE
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�X�R�A�v�Z
 *
 *	@param	u32 rhythm_score				���Y���`�F�b�N�̃X�R�A
 *	@param	aura_score							�I�[���`�F�b�N�̃X�R�A
 *	@param	u32 rhythm_minus				���Y���`�F�b�N�̌��_
 *	@param	aura_minus							�I�[���`�F�b�N�̌��_
 *	@param	MYSTATUS *cp_my_status	�^���l�̂��߂ɕK�v�ȏ��
 *	@param	MYSTATUS *cp_you_status	�^���l�̂��߂ɕK�v�ȏ��
 *	@param	heapID 
 *
 *	@return	�X�R�A
 */
//-----------------------------------------------------------------------------
static u32 RULE_CalcScore( u32 rhythm_score, u32 aura_score, u32 rhythm_minus, u32 aura_minus, const COMPATIBLE_STATUS *cp_my_status, const COMPATIBLE_STATUS *cp_you_status, BOOL is_init, HEAPID heapID )
{	
  u32 bio;
	s32 score;
  u32	add;

	//�v���C���[�̖��O����^���l���Z�o

  //�o�C�I���Y��
  bio = (RULE_CalcBioRhythm( cp_my_status ) + RULE_CalcBioRhythm( cp_you_status )) / 2;

  //�����_
	score	= (rhythm_score * 50/100) + (aura_score * 40/100) + (bio*10/100);

	//�^���l�v�Z�i�Q�^�̕����j
	{	
		fx32	rate;
    u32	name_score;
    const MYSTATUS  *cp_my   = (const MYSTATUS  *)cp_my_status->my_status;
    const MYSTATUS  *cp_you  = (const MYSTATUS  *)cp_you_status->my_status;

    name_score	= RULE_CalcNameScore( MyStatus_GetMyName(cp_my), MyStatus_GetMyName(cp_you) );
		rate	= FX32_CONST(name_score) / 150;

    if( is_init )
    {
      //����v���C����rate0.8�Œ�
      rate  = FX32_CONST(0.8F);
    }
    else
    { 
      rate	= MATH_CLAMP( rate , FX32_CONST(0.5F), FX32_CONST(1.0F) );
    }

		add	= ((100-score) * rate) >> FX32_SHIFT;
	}
  score	+= add;

  //�Ō�Ɍ��_
  score -= ( aura_minus + rhythm_minus );
  score = MATH_CLAMP( score, 0, 100 );

  OS_TPrintf( "�������ŏI�v�Z�� %d\n", score );
  OS_TPrintf( "���Y���_ %d ���_ -%d\n", rhythm_score, rhythm_minus );
  OS_TPrintf( "�I�[���_ %d ���_ -%d\n", aura_score, aura_minus );
  OS_TPrintf( "�o�C�I %d �Q�^ %d\n", bio, add );

	return score;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�^���l���v�Z
 *
 *	@param	STRBUF	*p_player1_name	���O�P
 *	@param	STRBUF	*p_player2_name	���O�Q
 *
 *	@return	�X�R�A
 */
//-----------------------------------------------------------------------------
static u32 RULE_CalcNameScore( const STRCODE	*cp_player1_name, const STRCODE	*cp_player2_name )
{	
	enum
	{	
		BIT_NUM	= 8,
	};

	const STRCODE	*cp_p1;
	const STRCODE	*cp_p2;
	
	u32 num1;
	u32 num2;
	u32 b1;
	u32 b2;
	u32 bit1;
	u32 bit2;
	u32 ans_cnt;
	u32 ans_max;
	u32 score;

	cp_p1	= cp_player1_name;
	cp_p2	= cp_player2_name;
	num1	= *cp_p1;
	num2	= *cp_p2;
  if( 0x30A1 <= num1 && num1 <= 0x30F4 )
  { 
    num1  -= 0x60;
  }
  if( 0x30A1 <= num2 && num2 <= 0x30F4 )
  { 
    num2  -= 0x60;
  }
	bit1	= MATH_GetMostOnebit( num1, BIT_NUM );
	bit2	= MATH_GetMostOnebit( num2, BIT_NUM );

	ans_max	= 0;
	ans_cnt	= 0;
	while( 1 )
	{
	 OS_Printf( "num1 0x%x bit1%d num2 0x%x bit2%d\n", num1, bit1, num2, bit2 );
	 if( bit1 == 0 )
	 {	
		cp_p1++;
		num1	= *cp_p1;

		if( num1 == GFL_STR_GetEOMCode() )
		{	
			break;
		}
    //�J�^�����ȕϊ�
    if( 0x30A1 <= num1 && num1 <= 0x30F4 )
    { 
      num1  -= 0x60;
    }


		bit1	= MATH_GetMostOnebit( num1, BIT_NUM );
	 }
	 if( bit2 == 0 )
	 {	
		cp_p2++;
		num2	= *cp_p2;
		if( num2 == GFL_STR_GetEOMCode() )
		{	
			break;
		}
    //�J�^�����ȕϊ�
    if( 0x30A1 <= num2 && num2 <= 0x30F4 )
    { 
      num2  -= 0x60;
    }

 		bit2	= MATH_GetMostOnebit( num2, BIT_NUM );
	 }

   if( bit1 != 0 && bit2 != 0 )
   { 

     b1	= ((num1) >> bit1) & 0x1;
     b2	= ((num2) >> bit2) & 0x1;

     //bit�̈�v�����`�F�b�N
     if( b1 == b2 )
     {	
       ans_cnt++;
     }
     ans_max++;

     //�r�b�g�����炷
     bit1--;
     bit2--;
   }
	}
	
	score	= 100*ans_cnt/ans_max;
	OS_Printf( "�S�̂̃r�b�g%d ��v%d �_��%d \n", ans_max, ans_cnt, score );

	return score;

}

//----------------------------------------------------------------------------
/**
 *	@brief	�ŏ�ʂ��P�̃r�b�g�𐔓���
 *
 *	@param	u32 x		�l
 *	@param	u8 bit	�r�b�g��
 *
 *	@return	�ŏ�ʂ̂P�̃r�b�g�̈ʒu
 */
//-----------------------------------------------------------------------------
static u32 MATH_GetMostOnebit( u32 x, u8 bit )
{	
	int i;
	for( i = bit-1; i != 0; i-- )
	{	
		if( (x >> i) & 0x1 )
		{
			break;
		}
	}

	return i;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�C�I���Y�����v�Z
 *
 *	@param	const COMPATIBLE_STATUS *cp_status  �X�e�[�^�X
 *
 *	@return
 */
//-----------------------------------------------------------------------------
#include "system/rtc_tool.h"
static u32 RULE_CalcBioRhythm( const COMPATIBLE_STATUS *cp_status )
{ 
  RTCDate date;
  GFL_RTC_GetDate( &date );
  return Irc_Compatible_SV_CalcBioRhythm( cp_status->barth_month, cp_status->barth_day, &date );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���Y���`�F�b�N�̃}�C�i�X�_���v�Z
 *
 *	@param	u32 cnt_diff  �����Ƒ���̍�
 *
 *	@return �}�C�i�X�_
 */
//-----------------------------------------------------------------------------
static u32 RULE_CalcRhythmMinus( u32 cnt_diff )
{ 
  static const sc_minus_tbl[] =
  { 
    0,
    2,
    4,
    6,
    8,
    10,
  };

  cnt_diff = MATH_IMin( cnt_diff, NELEMS(sc_minus_tbl) );

  return sc_minus_tbl[ cnt_diff ];
}
