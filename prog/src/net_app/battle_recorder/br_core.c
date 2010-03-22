//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_core.c
 *	@brief	�o�g�����R�[�_�[�R�A�����i�o�g�����R�[�h�v���C���[�ȊO�̕����j
 *	@author	Toru=Nagihashi
 *	@data		2009.11.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID

//�����̃��W���[��
#include "br_graphic.h"
#include "br_res.h"
#include "br_proc_sys.h"
#include "br_fade.h"
#include "br_sidebar.h"

//�e�v���Z�X
#include "br_start_proc.h"
#include "br_menu_proc.h"
#include "br_record_proc.h"
#include "br_btlsubway_proc.h"
#include "br_rndmatch_proc.h"
#include "br_bvrank_proc.h"
#include "br_bvsearch_proc.h"
#include "br_codein_proc.h"
#include "br_bvsend_proc.h"
#include "br_musicallook_proc.h"
#include "br_musicalsend_proc.h"

//�Z�[�u�f�[�^
#include "savedata/battlematch_savedata.h"
#include "savedata/battle_rec.h"
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"

//�O���Q��
#include "br_core.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���C�����[�N
//=====================================
typedef struct 
{
	//�O���t�B�b�N
	BR_GRAPHIC_WORK	*p_graphic;

	//���\�[�X�Ǘ�
	BR_RES_WORK			*p_res;

	//�v���Z�X�Ǘ�
	BR_PROC_SYS			*p_procsys;

  //�t�F�[�h
  BR_FADE_WORK    *p_fade;

  //�T�C�h�o�[
  BR_SIDEBAR_WORK *p_sidebar;

	//����
	BR_CORE_PARAM		*p_param;

  BR_MENU_BTLREC_DATA  btlrec_data;
} BR_CORE_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	BR�R�A�v���Z�X
//=====================================
static GFL_PROC_RESULT BR_CORE_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_CORE_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_CORE_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
///	�e�v���Z�X�̂��߂̏����֐�
//=====================================
//�N��
static void BR_START_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_START_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//���j���[
static void BR_MENU_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_MENU_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//���R�[�h
static void BR_RECORD_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_RECORD_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//�o�g���T�u�E�F�C
static void BR_BTLSUBWAY_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_BTLSUBWAY_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//�����_���}�b�`
static void BR_RNDMATCH_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_RNDMATCH_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//�o�g���r�f�I30��
static void BR_BVRANK_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_BVRANK_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//�ڂ����T��
static void BR_BVSEARCH_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_BVSEARCH_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//�r�f�I�i���o�[
static void BR_CODEIN_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_CODEIN_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//�r�f�I���M
static void BR_BVSEND_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_BVSEND_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//�~���[�W�J���V���b�g�ʐ^���݂�
static void BR_MUSICALLOOK_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_MUSICALLOOK_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//�~���[�W�J���V���b�g�ʐ^�𑗂�
static void BR_MUSICALSEND_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_MUSICALSEND_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	�o�g�����R�[�_�[�R�A�v���Z�X
//=====================================
const GFL_PROC_DATA BR_CORE_ProcData =
{	
	BR_CORE_PROC_Init,
	BR_CORE_PROC_Main,
	BR_CORE_PROC_Exit,
};

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�v���Z�X�q������
//=====================================
static const BR_PROC_SYS_DATA sc_procdata_tbl[BR_PROCID_MAX]	=
{	
	//	BR_PROCID_START,		//�N�����
	{	
		&BR_START_ProcData,
		sizeof( BR_START_PROC_PARAM ),
		BR_START_PROC_BeforeFunc,
		BR_START_PROC_AfterFunc,
	},
	//	BR_PROCID_MENU,			//���j���[���
	{	
		&BR_MENU_ProcData,
		sizeof( BR_MENU_PROC_PARAM ),
		BR_MENU_PROC_BeforeFunc,
		BR_MENU_PROC_AfterFunc,
	},
	//	BR_PROCID_RECORD,		//�^��L�^���
	{	
		&BR_RECORD_ProcData,
		sizeof( BR_RECORD_PROC_PARAM ),
		BR_RECORD_PROC_BeforeFunc,
		BR_RECORD_PROC_AfterFunc,
	},
	//	BR_PROCID_BTLSUBWAY,//�o�g���T�u�E�F�C���щ��
	{	
		&BR_BTLSUBWAY_ProcData,
		sizeof( BR_BTLSUBWAY_PROC_PARAM ),
		BR_BTLSUBWAY_PROC_BeforeFunc,
		BR_BTLSUBWAY_PROC_AfterFunc,
	},
	//	BR_PROCID_RNDMATCH,	//�����_���}�b�`���щ��
	{	
		&BR_RNDMATCH_ProcData,
		sizeof( BR_RNDMATCH_PROC_PARAM ),
		BR_RNDMATCH_PROC_BeforeFunc,
		BR_RNDMATCH_PROC_AfterFunc,
	},
	//	BR_PROCID_BV_RANK,	//�o�g���r�f�I30����ʁi�ŐV�A�ʐM�ΐ�A�T�u�E�F�C�j
	{	
		&BR_BVRANK_ProcData,
		sizeof( BR_BVRANK_PROC_PARAM ),
		BR_BVRANK_PROC_BeforeFunc,
		BR_BVRANK_PROC_AfterFunc,
	},
	//	BR_PROCID_BV_SEARCH,//�ڂ����T�����
	{	
		&BR_BVSEARCH_ProcData,
		sizeof( BR_BVSEARCH_PROC_PARAM ),
		BR_BVSEARCH_PROC_BeforeFunc,
		BR_BVSEARCH_PROC_AfterFunc,
	},
	//	BR_PROCID_CODEIN		,//�o�g���r�f�I�i���o�[���͉��
	{	
		&BR_CODEIN_ProcData,
		sizeof( BR_CODEIN_PROC_PARAM ),
		BR_CODEIN_PROC_BeforeFunc,
		BR_CODEIN_PROC_AfterFunc,
	},
	//	BR_PROCID_BV_SEND,	//�o�g���r�f�I���M���
	{	
		&BR_BVSEND_ProcData,
		sizeof( BR_BVSEND_PROC_PARAM ),
		BR_BVSEND_PROC_BeforeFunc,
		BR_BVSEND_PROC_AfterFunc,
	},
	//	BR_PROCID_MUSICAL_LOOK,	//�~���[�W�J���V���b�g	�ʐ^��������
	{	
		&BR_MUSICALLOOK_ProcData,
		sizeof( BR_MUSICALLOOK_PROC_PARAM ),
		BR_MUSICALLOOK_PROC_BeforeFunc,
		BR_MUSICALLOOK_PROC_AfterFunc,
	},
	//	BR_PROCID_MUSICAL_SEND,	//�~���[�W�J���V���b�g	�ʐ^�𑗂���
	{	
		&BR_MUSICALSEND_ProcData,
		sizeof( BR_MUSICALSEND_PROC_PARAM ),
		BR_MUSICALSEND_PROC_BeforeFunc,
		BR_MUSICALSEND_PROC_AfterFunc,
	},
};

//=============================================================================
/**
 *					BR�R�A�v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[�R�A	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_CORE_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_CORE_WORK	*p_wk;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BATTLE_RECORDER_CORE, 0x60000 );
//	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BATTLE_RECORDER_CORE, 0x30000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_CORE_WORK), HEAPID_BATTLE_RECORDER_CORE );
	GFL_STD_MemClear( p_wk, sizeof(BR_CORE_WORK) );	
	p_wk->p_param		= p_param_adrs;

	//�O���t�B�b�N������
	p_wk->p_graphic	= BR_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_BATTLE_RECORDER_CORE );
	p_wk->p_res			= BR_RES_Init( HEAPID_BATTLE_RECORDER_CORE );
	BR_RES_LoadBG( p_wk->p_res, BR_RES_BG_START_M, HEAPID_BATTLE_RECORDER_CORE );
	BR_RES_LoadBG( p_wk->p_res, BR_RES_BG_START_S, HEAPID_BATTLE_RECORDER_CORE );

	//�R�A�v���Z�X������
  p_wk->p_procsys	= BR_PROC_SYS_Init( BR_PROCID_START, sc_procdata_tbl, 
      BR_PROCID_MAX, p_wk, HEAPID_BATTLE_RECORDER_CORE );

  //�t�F�[�h�v���Z�X
  p_wk->p_fade  = BR_FADE_Init( HEAPID_BATTLE_RECORDER_CORE );
  BR_FADE_PALETTE_Copy( p_wk->p_fade );  //BR_RES_Init�ł��ł���̂�
  BR_FADE_PALETTE_SetColor( p_wk->p_fade, BR_FADE_COLOR_BLUE );
  if( p_wk->p_param->mode == BR_CORE_MODE_INIT )
  { 
    BR_FADE_PALETTE_TransColor( p_wk->p_fade, BR_FADE_DISPLAY_BOTH );
  }

  //�T�C�h�o�[�쐬
  {
    GFL_CLUNIT  *p_clunit = BR_GRAPHIC_GetClunit( p_wk->p_graphic );
    p_wk->p_sidebar = BR_SIDEBAR_Init( p_clunit, p_wk->p_fade, p_wk->p_res, HEAPID_BATTLE_RECORDER_CORE );
  }

  //���j���[�Ŏg�p����^��f�[�^
  {	
    int i;
    LOAD_RESULT result;
    SAVE_CONTROL_WORK *p_sv;
    GDS_PROFILE_PTR p_profile;
    
    p_sv = GAMEDATA_GetSaveControlWork( p_wk->p_param->p_param->p_gamedata ); 

    //�ݒ�\���̍쐬
    for( i = 0; i < BR_BTLREC_DATA_NUM; i++ )
    {
      BattleRec_Load( p_sv, GFL_HEAP_LOWID( HEAPID_BATTLE_RECORDER_CORE ), &result, i ); 
      if( result == LOAD_RESULT_OK )
      { 
        NAGI_Printf( "�퓬�^��ǂݍ��� %d\n", i );
        p_profile = BattleRec_GDSProfilePtrGet();
        p_wk->btlrec_data.is_valid[i] = TRUE;
        p_wk->btlrec_data.p_name[i]   = GDS_Profile_CreateNameString( p_profile, HEAPID_BATTLE_RECORDER_CORE );
        p_wk->btlrec_data.sex[i]      = GDS_Profile_GetSex( p_profile );
      }
    }
  }

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[�R�A	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_CORE_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_CORE_WORK	*p_wk	= p_wk_adrs;

  { 
    int i;
    //�ݒ�\���̔j��
    for( i = 0; i < BR_BTLREC_DATA_NUM; i++ )
    { 
      if( p_wk->btlrec_data.is_valid[i] )
      { 
        GFL_STR_DeleteBuffer( p_wk->btlrec_data.p_name[i] );
      }
    }
	}


  //�T�C�h�o�[�j��
  BR_SIDEBAR_Exit( p_wk->p_sidebar, p_wk->p_res );

  //�t�F�[�h�v���Z�X�j��
  BR_FADE_Exit( p_wk->p_fade );

	//�R�A�v���Z�X�j��
	BR_PROC_SYS_Exit(	p_wk->p_procsys );

	//�O���t�B�b�N�j��
	BR_RES_UnLoadBG( p_wk->p_res, BR_RES_BG_START_M );
	BR_RES_UnLoadBG( p_wk->p_res, BR_RES_BG_START_S );
	BR_RES_Exit( p_wk->p_res );
	BR_GRAPHIC_Exit( p_wk->p_graphic );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_BATTLE_RECORDER_CORE );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[�R�A	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_CORE_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  enum
  { 
		SEQ_INIT,
		SEQ_FADEIN,
		SEQ_FADEIN_WAIT,
		SEQ_MAIN,
		SEQ_FADEOUT,
		SEQ_FADEOUT_WAIT,
		SEQ_EXIT,
  };

	BR_CORE_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
	{	
	case SEQ_INIT:
    *p_seq	= SEQ_FADEIN;
		break;

	case SEQ_FADEIN:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_MAIN;
		}
		break;

	case SEQ_MAIN:
		{
      //�t�F�[�h�v���Z�X����
      BR_FADE_Main( p_wk->p_fade );

      //�T�C�h�o�[����
      BR_SIDEBAR_Main( p_wk->p_sidebar );

      //�v���Z�X����
      BR_PROC_SYS_Main( p_wk->p_procsys );

      //�`��
      BR_GRAPHIC_2D_Draw( p_wk->p_graphic );

			if( BR_PROC_SYS_IsEnd( p_wk->p_procsys ) )
			{	
        *p_seq	= SEQ_FADEOUT;
			}
		}
		break;

	case SEQ_FADEOUT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_EXIT;
		}
		break;

	case SEQ_EXIT:
		return GFL_PROC_RES_FINISH;
	}

  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *					�e�v���Z�X�ڑ��p
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N�����O�֐�
 *
 *	@param	void *p_param_adrs	�A���P�[�g���ꂽ�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 *	@param	void *cp_pre_param	���̃v���Z�X�̑O�̈������[�N
 *	@param	preID								���̃v���Z�X�̑O��ID
 */
//-----------------------------------------------------------------------------
static void BR_START_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_START_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK					*p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
  p_param->p_sidebar  = p_wk->p_sidebar;

  if( preID == BR_PROCID_MENU )
  { 
    p_param->mode = BR_START_PROC_MODE_CLOSE;
  }
  else
  { 
      
    if( p_wk->p_param->mode == BR_CORE_MODE_INIT )
    { 
      p_param->mode = BR_START_PROC_MODE_OPEN;
    }
    else
    { 
      p_param->mode = BR_START_PROC_MODE_NONE;
    }
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N������֐�
 *
 *	@param	void *p_param_adrs	�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 */
//-----------------------------------------------------------------------------
static void BR_START_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 

}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N�����O�֐�
 *
 *	@param	void *p_param_adrs	�A���P�[�g���ꂽ�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 *	@param	void *cp_pre_param	���̃v���Z�X�̑O�̈������[�N
 *	@param	preID								���̃v���Z�X�̑O��ID
 */
//-----------------------------------------------------------------------------
static void BR_MENU_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{	
	BR_MENU_PROC_PARAM	*p_param	= p_param_adrs;
	BR_CORE_WORK				*p_wk			= p_wk_adrs;

  //�o�g������߂��Ă����Ƃ�
  if( p_wk->p_param->mode == BR_CORE_MODE_RETURN )
  {
    //����
    switch( p_wk->p_param->p_param->mode )
    { 
    case BR_MODE_BROWSE:
      p_param->menuID			= BR_BROWSE_MENUID_BTLVIDEO;
      break;

    case BR_MODE_GLOBAL_BV:
      p_param->menuID			= BR_BTLVIDEO_MENUID_RANK;
      break;

    case BR_MODE_GLOBAL_MUSICAL:
      //���Ȃ�
      p_param->menuID			= BR_MUSICAL_MENUID_TOP;
      break;
    }
  }
  else
  { 
    //�e�v���Z�X����߂��Ă���Ƃ�
    switch( preID )
    {	
    case BR_PROCID_RECORD:
      { 
        const BR_RECORD_PROC_PARAM	*cp_record_param	= cp_pre_param;
        if( cp_record_param->mode == BR_RECODE_PROC_MY )
        {	
          p_param->menuID			= BR_BROWSE_MENUID_BTLVIDEO;
        }
        else
        {
          p_param->menuID			= BR_BROWSE_MENUID_OTHER_RECORD;
        }
      }
      break;

    case BR_PROCID_BV_RANK:
      p_param->menuID = BR_BTLVIDEO_MENUID_RANK;
      break;

    case BR_PROCID_BV_SEARCH:
      /* fallthrough */
    case BR_PROCID_CODEIN:
      p_param->menuID = BR_BTLVIDEO_MENUID_LOOK;
      break;

    default:
      //����
      switch( p_wk->p_param->p_param->mode )
      { 
      case BR_MODE_BROWSE:
        p_param->menuID			= BR_BROWSE_MENUID_TOP;
        break;

      case BR_MODE_GLOBAL_BV:
        p_param->menuID			= BR_BTLVIDEO_MENUID_TOP;
        break;

      case BR_MODE_GLOBAL_MUSICAL:
        p_param->menuID			= BR_MUSICAL_MENUID_TOP;
        break;
      }
    }
  }
	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
  p_param->cp_btlrec  = &p_wk->btlrec_data;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N������֐�
 *
 *	@param	void *p_param_adrs	�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 */
//-----------------------------------------------------------------------------
static void BR_MENU_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{	
  BR_MENU_PROC_PARAM	*p_param	= p_param_adrs;
	BR_CORE_WORK				*p_wk			= p_wk_adrs;

}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N�����O�֐�
 *
 *	@param	void *p_param_adrs	�A���P�[�g���ꂽ�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 *	@param	void *cp_pre_param	���̃v���Z�X�̑O�̈������[�N
 *	@param	preID								���̃v���Z�X�̑O��ID
 */
//-----------------------------------------------------------------------------
static void BR_RECORD_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{	
	BR_RECORD_PROC_PARAM			*p_param	= p_param_adrs;
	BR_CORE_WORK							*p_wk			= p_wk_adrs;

	//���j���[�ȊO���痈�Ȃ�

  p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
  p_param->p_procsys	= p_wk->p_procsys;
  p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
  p_param->p_profile  = BattleRec_GDSProfilePtrGet();
  p_param->p_header   = BattleRec_HeaderPtrGet();
  switch( preID )
  { 
  case BR_PROCID_MENU:
    { 
      const BR_MENU_PROC_PARAM	*cp_menu_param	= cp_pre_param;
      p_param->mode				= cp_menu_param->next_mode;
    }
    break;

  case BR_PROCID_BV_RANK:
    { 
      //@todo�Ƃ肠�������͎���
      p_param->mode       = BR_RECODE_PROC_MY;
    }
    break;

  case BR_PROCID_CODEIN:
    { 
      //@todo�Ƃ肠�������͎���
      p_param->mode       = BR_RECODE_PROC_MY;
    }
    break;

  default:
    GF_ASSERT_MSG( 0, "���j���[�ƃ����L���O�A�R�[�h���͈ȊO����͗��Ȃ� %d", preID );
    break;
  }

  //�ǂݍ���
  {
    SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_wk->p_param->p_param->p_gamedata );

    LOAD_RESULT result;
    BattleRec_Load( p_sv_ctrl, HEAPID_BATTLE_RECORDER_SYS, &result, p_param->mode ); 
    if( result == LOAD_RESULT_OK )
    { 
      if( p_param->mode == LOADDATA_MYREC )
      { 
        GDS_Profile_MyDataSet(p_param->p_profile, p_sv_ctrl);
      }
      NAGI_Printf( "�퓬�^��ǂݍ��� %d\n", p_param->mode );
    }
    else
    { 
      GF_ASSERT(0);
    }
  }
  
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N������֐�
 *
 *	@param	void *p_param_adrs	�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 */
//-----------------------------------------------------------------------------
static void BR_RECORD_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{	
  BR_RECORD_PROC_PARAM			*p_param	= p_param_adrs;
	BR_CORE_WORK							*p_wk			= p_wk_adrs;
  
  switch( p_param->ret )
  { 
  case BR_RECORD_RETURN_BTLREC:
    p_wk->p_param->ret = BR_CORE_RETURN_BTLREC;
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N�����O�֐�
 *
 *	@param	void *p_param_adrs	�A���P�[�g���ꂽ�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 *	@param	void *cp_pre_param	���̃v���Z�X�̑O�̈������[�N
 *	@param	preID								���̃v���Z�X�̑O��ID
 */
//-----------------------------------------------------------------------------
static void BR_BTLSUBWAY_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{	
	BR_BTLSUBWAY_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK							*p_wk			= p_wk_adrs;
	const BR_MENU_PROC_PARAM	*cp_menu_param	= cp_pre_param;
  SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_wk->p_param->p_param->p_gamedata );

	//���j���[�ȊO���痈�Ȃ�
	GF_ASSERT_MSG( preID == BR_PROCID_MENU, "���j���[�ȊO����͗��Ȃ� %d", preID );
/*
	switch( cp_menu_param->next_data )
	{	
		
	}
*/
	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
  p_param->p_subway   = SaveControl_DataPtrGet( p_sv_ctrl, GMDATA_ID_BSUBWAY_SCOREDATA );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N������֐�
 *
 *	@param	void *p_param_adrs	�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 */
//-----------------------------------------------------------------------------
static void BR_BTLSUBWAY_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{	

}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N�����O�֐�
 *
 *	@param	void *p_param_adrs	�A���P�[�g���ꂽ�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 *	@param	void *cp_pre_param	���̃v���Z�X�̑O�̈������[�N
 *	@param	preID								���̃v���Z�X�̑O��ID
 */
//-----------------------------------------------------------------------------
static void BR_RNDMATCH_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{	
	BR_RNDMATCH_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK							*p_wk			= p_wk_adrs;
	const BR_MENU_PROC_PARAM	*cp_menu_param	= cp_pre_param;
  SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_wk->p_param->p_param->p_gamedata );

	//���j���[�ȊO���痈�Ȃ�
	GF_ASSERT_MSG( preID == BR_PROCID_MENU, "���j���[�ȊO����͗��Ȃ� %d", preID );

	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
  {
    BATTLEMATCH_DATA  *p_btlmatch_sv  = SaveData_GetBattleMatch( p_sv_ctrl );
    p_param->p_rndmatch = BATTLEMATCH_GetRndMatch( p_btlmatch_sv );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N������֐�
 *
 *	@param	void *p_param_adrs	�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 */
//-----------------------------------------------------------------------------
static void BR_RNDMATCH_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{	

}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N�����O�֐�
 *
 *	@param	void *p_param_adrs	�A���P�[�g���ꂽ�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 *	@param	void *cp_pre_param	���̃v���Z�X�̑O�̈������[�N
 *	@param	preID								���̃v���Z�X�̑O��ID
 */
//-----------------------------------------------------------------------------
static void BR_BVRANK_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_BVRANK_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						*p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N������֐�
 *
 *	@param	void *p_param_adrs	�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 */
//-----------------------------------------------------------------------------
static void BR_BVRANK_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 

}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N�����O�֐�
 *
 *	@param	void *p_param_adrs	�A���P�[�g���ꂽ�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 *	@param	void *cp_pre_param	���̃v���Z�X�̑O�̈������[�N
 *	@param	preID								���̃v���Z�X�̑O��ID
 */
//-----------------------------------------------------------------------------
static void BR_BVSEARCH_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_BVSEND_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						*p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N������֐�
 *
 *	@param	void *p_param_adrs	�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 */
//-----------------------------------------------------------------------------
static void BR_BVSEARCH_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 

}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N�����O�֐�
 *
 *	@param	void *p_param_adrs	�A���P�[�g���ꂽ�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 *	@param	void *cp_pre_param	���̃v���Z�X�̑O�̈������[�N
 *	@param	preID								���̃v���Z�X�̑O��ID
 */
//-----------------------------------------------------------------------------
static void BR_CODEIN_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_CODEIN_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						*p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N������֐�
 *
 *	@param	void *p_param_adrs	�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 */
//-----------------------------------------------------------------------------
static void BR_CODEIN_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 

}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N�����O�֐�
 *
 *	@param	void *p_param_adrs	�A���P�[�g���ꂽ�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 *	@param	void *cp_pre_param	���̃v���Z�X�̑O�̈������[�N
 *	@param	preID								���̃v���Z�X�̑O��ID
 */
//-----------------------------------------------------------------------------
static void BR_BVSEND_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_BVSEND_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						*p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N������֐�
 *
 *	@param	void *p_param_adrs	�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 */
//-----------------------------------------------------------------------------
static void BR_BVSEND_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 


}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N�����O�֐�
 *
 *	@param	void *p_param_adrs	�A���P�[�g���ꂽ�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 *	@param	void *cp_pre_param	���̃v���Z�X�̑O�̈������[�N
 *	@param	preID								���̃v���Z�X�̑O��ID
 */
//-----------------------------------------------------------------------------
static void BR_MUSICALLOOK_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_MUSICALLOOK_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						    *p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_graphic	= p_wk->p_graphic;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N������֐�
 *
 *	@param	void *p_param_adrs	�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 */
//-----------------------------------------------------------------------------
static void BR_MUSICALLOOK_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 

}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N�����O�֐�
 *
 *	@param	void *p_param_adrs	�A���P�[�g���ꂽ�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 *	@param	void *cp_pre_param	���̃v���Z�X�̑O�̈������[�N
 *	@param	preID								���̃v���Z�X�̑O��ID
 */
//-----------------------------------------------------------------------------
static void BR_MUSICALSEND_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_MUSICALSEND_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						    *p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_graphic	= p_wk->p_graphic;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N������֐�
 *
 *	@param	void *p_param_adrs	�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 */
//-----------------------------------------------------------------------------
static void BR_MUSICALSEND_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 

}
