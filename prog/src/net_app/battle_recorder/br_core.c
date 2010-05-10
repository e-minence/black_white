//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_core.c
 *	@brief	�o�g�����R�[�_�[�R�A����
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
#include "src\field\field_sound.h"

//�����̃��W���[��
#include "br_graphic.h"
#include "br_res.h"
#include "br_proc_sys.h"
#include "br_fade.h"
#include "br_sidebar.h"
#include "br_net.h"
#include "br_snd.h"

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
#include "br_bvdelete_proc.h"
#include "br_bvsave_proc.h"
#include "br_musicallook_proc.h"
#include "br_musicalsend_proc.h"

//�Z�[�u�f�[�^
#include "savedata/battlematch_savedata.h"
#include "savedata/battle_rec.h"
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"

//�f�o�b�O
#include "debug/debug_nagihashi.h"

//�O���Q��
#include "br_core.h"

//�I�[�o�[���C
FS_EXTERN_OVERLAY( battle_recorder_browse );
FS_EXTERN_OVERLAY( battle_recorder_musical_look );
FS_EXTERN_OVERLAY( battle_recorder_musical_send );
FS_EXTERN_OVERLAY( gds_comm );

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

  //�ʐM
  BR_NET_WORK     *p_net;

	//����
	BR_CORE_PARAM		*p_param;

  //�ȉ��v���Z�X�Ԃ̌q��
  BR_BVRANK_MODE  rank_mode;
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
//�r�f�I����
static void BR_BVDELETE_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_BVDELETE_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//�r�f�I�ۑ�
static void BR_BVSAVE_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_BVSAVE_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//�~���[�W�J���V���b�g�ʐ^���݂�
static void BR_MUSICALLOOK_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_MUSICALLOOK_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//�~���[�W�J���V���b�g�ʐ^�𑗂�
static void BR_MUSICALSEND_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_MUSICALSEND_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
///	���̑�
//=====================================
static void Br_Core_CheckSaveData( BR_SAVE_INFO *p_saveinfo, BOOL is_onece, GAMEDATA *p_gamedata, HEAPID heapID );

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
    FS_OVERLAY_ID( battle_recorder_browse ),
		BR_START_PROC_BeforeFunc,
		BR_START_PROC_AfterFunc,
	},
	//	BR_PROCID_MENU,			//���j���[���
	{	
		&BR_MENU_ProcData,
		sizeof( BR_MENU_PROC_PARAM ),
    FS_OVERLAY_ID( battle_recorder_browse ),
		BR_MENU_PROC_BeforeFunc,
		BR_MENU_PROC_AfterFunc,
	},
	//	BR_PROCID_RECORD,		//�^��L�^���
	{	
		&BR_RECORD_ProcData,
		sizeof( BR_RECORD_PROC_PARAM ),
    FS_OVERLAY_ID( battle_recorder_browse ),
		BR_RECORD_PROC_BeforeFunc,
		BR_RECORD_PROC_AfterFunc,
	},
	//	BR_PROCID_BTLSUBWAY,//�o�g���T�u�E�F�C���щ��
	{	
		&BR_BTLSUBWAY_ProcData,
		sizeof( BR_BTLSUBWAY_PROC_PARAM ),
    FS_OVERLAY_ID( battle_recorder_browse ),
		BR_BTLSUBWAY_PROC_BeforeFunc,
		BR_BTLSUBWAY_PROC_AfterFunc,
	},
	//	BR_PROCID_RNDMATCH,	//�����_���}�b�`���щ��
	{	
		&BR_RNDMATCH_ProcData,
		sizeof( BR_RNDMATCH_PROC_PARAM ),
    FS_OVERLAY_ID( battle_recorder_browse ),
		BR_RNDMATCH_PROC_BeforeFunc,
		BR_RNDMATCH_PROC_AfterFunc,
	},
	//	BR_PROCID_BV_RANK,	//�o�g���r�f�I30����ʁi�ŐV�A�ʐM�ΐ�A�T�u�E�F�C�j
	{	
		&BR_BVRANK_ProcData,
		sizeof( BR_BVRANK_PROC_PARAM ),
    FS_OVERLAY_ID( battle_recorder_browse ),
		BR_BVRANK_PROC_BeforeFunc,
		BR_BVRANK_PROC_AfterFunc,
	},
	//	BR_PROCID_BV_SEARCH,//�ڂ����T�����
	{	
		&BR_BVSEARCH_ProcData,
		sizeof( BR_BVSEARCH_PROC_PARAM ),
    FS_OVERLAY_ID( battle_recorder_browse ),
		BR_BVSEARCH_PROC_BeforeFunc,
		BR_BVSEARCH_PROC_AfterFunc,
	},
	//	BR_PROCID_CODEIN		,//�o�g���r�f�I�i���o�[���͉��
	{	
		&BR_CODEIN_ProcData,
		sizeof( BR_CODEIN_PROC_PARAM ),
    FS_OVERLAY_ID( battle_recorder_browse ),
		BR_CODEIN_PROC_BeforeFunc,
		BR_CODEIN_PROC_AfterFunc,
	},
	//	BR_PROCID_BV_SEND,	//�o�g���r�f�I���M���
	{	
		&BR_BVSEND_ProcData,
		sizeof( BR_BVSEND_PROC_PARAM ),
    FS_OVERLAY_ID( battle_recorder_browse ),
		BR_BVSEND_PROC_BeforeFunc,
		BR_BVSEND_PROC_AfterFunc,
	},
	//BR_PROCID_BV_DELETE,	  //�o�g���r�f�I�������
  { 
		&BR_BVDELETE_ProcData,
		sizeof( BR_BVDELETE_PROC_PARAM ),
    FS_OVERLAY_ID( battle_recorder_browse ),
		BR_BVDELETE_PROC_BeforeFunc,
		BR_BVDELETE_PROC_AfterFunc,
  },
  //BR_PROCID_BV_SAVE,    //�o�g���r�f�I�ۑ����
  { 
		&BR_BVSAVE_ProcData,
		sizeof( BR_BVSAVE_PROC_PARAM ),
    FS_OVERLAY_ID( battle_recorder_browse ),
		BR_BVSAVE_PROC_BeforeFunc,
		BR_BVSAVE_PROC_AfterFunc,
  },
	//	BR_PROCID_MUSICAL_LOOK,	//�~���[�W�J���V���b�g	�ʐ^��������
	{	
		&BR_MUSICALLOOK_ProcData,
		sizeof( BR_MUSICALLOOK_PROC_PARAM ),
    FS_OVERLAY_ID( battle_recorder_musical_look ),
		BR_MUSICALLOOK_PROC_BeforeFunc,
		BR_MUSICALLOOK_PROC_AfterFunc,
	},
	//	BR_PROCID_MUSICAL_SEND,	//�~���[�W�J���V���b�g	�ʐ^�𑗂���
	{	
		&BR_MUSICALSEND_ProcData,
		sizeof( BR_MUSICALSEND_PROC_PARAM ),
    FS_OVERLAY_ID( battle_recorder_musical_send ),
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
  BR_GRAPHIC_SETUP_TYPE graphic_type;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BATTLE_RECORDER_CORE, 0x65000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_CORE_WORK), HEAPID_BATTLE_RECORDER_CORE );
	GFL_STD_MemClear( p_wk, sizeof(BR_CORE_WORK) );	
	p_wk->p_param		= p_param_adrs;
  p_wk->p_param->p_param->result  = BR_RESULT_RETURN;

	//�O���t�B�b�N������
  if( p_wk->p_param->p_param->mode == BR_MODE_GLOBAL_MUSICAL )
  { 
    graphic_type  = BR_GRAPHIC_SETUP_3D;
  }
  else
  { 
    graphic_type  = BR_GRAPHIC_SETUP_2D;
  }
	p_wk->p_graphic	= BR_GRAPHIC_Init( graphic_type, GX_DISP_SELECT_MAIN_SUB, HEAPID_BATTLE_RECORDER_CORE );

  //�������\�[�X�ǂݍ���
  {
    MISC  *p_misc = GAMEDATA_GetMiscWork( p_wk->p_param->p_param->p_gamedata );
    p_wk->p_res			= BR_RES_Init( 
        MISC_GetBattleRecorderColor( p_misc ),
        p_wk->p_param->p_param->mode == BR_MODE_BROWSE,
        HEAPID_BATTLE_RECORDER_CORE );
  }
	BR_RES_LoadBG( p_wk->p_res, BR_RES_BG_START_M, HEAPID_BATTLE_RECORDER_CORE );
	BR_RES_LoadBG( p_wk->p_res, BR_RES_BG_START_S, HEAPID_BATTLE_RECORDER_CORE );

	//�R�A�v���Z�X������
  p_wk->p_procsys	= BR_PROC_SYS_Init( BR_PROCID_START, sc_procdata_tbl, 
      BR_PROCID_MAX, p_wk, &p_wk->p_param->p_data->proc_recovery, HEAPID_BATTLE_RECORDER_CORE );

  //�t�F�[�h�v���Z�X(�p���b�g�t�F�[�h���g���̂Ń��\�[�X��ǂ񂾂��ƂɃR�s�[������F�ݒ肵���肵�Ă���)
  p_wk->p_fade  = BR_FADE_Init( HEAPID_BATTLE_RECORDER_CORE );
  BR_FADE_PALETTE_Copy( p_wk->p_fade );  //BR_RES_Init�Ń��\�[�X��VRAM�ɓǂ�ł���̂ŁA��������R�s�[
  BR_FADE_PALETTE_SetColor( p_wk->p_fade, BR_RES_GetFadeColor( p_wk->p_res ) );
  if( p_wk->p_param->mode == BR_CORE_MODE_INIT )
  { 
    BR_FADE_PALETTE_TransColor( p_wk->p_fade, BR_FADE_DISPLAY_BOTH );
  }

  //�T�C�h�o�[�쐬
  {
    GFL_CLUNIT  *p_clunit = BR_GRAPHIC_GetClunit( p_wk->p_graphic );
    p_wk->p_sidebar = BR_SIDEBAR_Init( p_clunit, p_wk->p_fade, p_wk->p_res, HEAPID_BATTLE_RECORDER_CORE );
  }

  //���j���[�Ŏg�p����^��f�[�^���`�F�b�N
  Br_Core_CheckSaveData( &p_wk->p_param->p_data->rec_saveinfo,
      TRUE, p_wk->p_param->p_param->p_gamedata, HEAPID_BATTLE_RECORDER_CORE );

  //GDS�̂ݒʐMON
  { 
    if( p_wk->p_param->p_param->mode != BR_MODE_BROWSE )
    { 
      p_wk->p_net = BR_NET_Init( 
          p_wk->p_param->p_param->p_gamedata,
          p_wk->p_param->p_param->p_svl,
          HEAPID_BATTLE_RECORDER_CORE );
    }
    else
    { 
      //���̃I�[�o�[���C��gds���[�h�̂Ƃ��ɓǂ܂�邪�A
      //battle_rec���삪���邽�߁A�ʐM���g��Ȃ��Ƃ��ɂł�
      //�I�[�o�[���C�ɒu��
      GFL_OVERLAY_Load( FS_OVERLAY_ID(gds_comm) );
    }

    //�u���E�U���[�h�ł��ʐM�A�C�R��������
    if( GFL_NET_IsInit() )
    { 
      GFL_NET_WirelessSetForceXYPos(GFL_WICON_POSX,GFL_WICON_POSY);
      GFL_NET_WirelessIconEasy_HoldLCD( FALSE, HEAPID_BATTLE_RECORDER_CORE );
      GFL_NET_ReloadIcon();
    }
  }

  //�u���E�U���[�h�̂݃{�����[����������
  //�o�g������̖߂�̂Ƃ���br_main.c��BR_BATTLE_FreeParam�ōs���Ă���̂ōs��Ȃ�
  if( p_wk->p_param->p_param->mode == BR_MODE_BROWSE )
  { 
    if( p_wk->p_param->mode != BR_CORE_MODE_RETURN )
    { 
      FIELD_SOUND *p_fld_snd  = GAMEDATA_GetFieldSound( p_wk->p_param->p_param->p_gamedata );
      FSND_HoldBGMVolume_inApp( p_fld_snd );
    }
  }
  else
  { 
    //����ȊO�̃��[�h��GDS�̋Ȃ�������
    PMSND_PlayBGM( BR_SND_BGM_MAIN );
  }

  //�퓬���痈���ꍇ�́AALPHA��ON�ɂ��A�T�C�h�o�[��Ԃ��J������Ԃɂ���
  if( p_wk->p_param->mode == BR_CORE_MODE_RETURN )
  { 
    BR_SIDEBAR_SetShakePos( p_wk->p_sidebar );
    BR_SIDEBAR_StartShake( p_wk->p_sidebar );
    BR_FADE_ALPHA_SetAlpha( p_wk->p_fade, BR_FADE_DISPLAY_BOTH, 0 );
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

  //�u���E�U���[�h�̂݃{�����[�������ɖ߂�
  if( p_wk->p_param->p_param->mode == BR_MODE_BROWSE )
  { 
    FIELD_SOUND *p_fld_snd  = GAMEDATA_GetFieldSound( p_wk->p_param->p_param->p_gamedata );
    FSND_ReleaseBGMVolume_inApp( p_fld_snd );
  }

  //�l�b�g�j��
  if( p_wk->p_net )
  { 
    if( BR_NET_SYSERR_RETURN_DISCONNECT == BR_NET_GetSysError( p_wk->p_net ) )
    { 
      p_wk->p_param->p_param->result  = BR_RESULT_NET_ERROR;
    }
    BR_NET_Exit( p_wk->p_net );
  }
  else
  { 
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( gds_comm ) );
  }

  //�T�C�h�o�[�j��
  BR_SIDEBAR_Exit( p_wk->p_sidebar, p_wk->p_res );

  //�t�F�[�h�v���Z�X�j��
  BR_FADE_Exit( p_wk->p_fade );

	//�R�A�v���Z�X�j��
	BR_PROC_SYS_Exit(	p_wk->p_procsys );


  //RES�j���O�ɐF�ۑ�
  if( p_wk->p_param->p_param->mode == BR_MODE_BROWSE )
  { 
    MISC  *p_misc = GAMEDATA_GetMiscWork( p_wk->p_param->p_param->p_gamedata );
    MISC_SetBattleRecorderColor( p_misc, BR_RES_GetColor( p_wk->p_res ) );
  }

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

      //�ʐM
      if( p_wk->p_net )
      { 
        BR_NET_Main( p_wk->p_net );
      }

      //�I��
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

  p_param->fade_type = BR_FADE_TYPE_ALPHA_BG012OBJ;
  p_param->p_result   = &p_wk->p_param->p_param->result;
  p_param->p_btn_recovery = &p_wk->p_param->p_data->btn_recovery;

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
        switch( cp_record_param->mode )
        {	
        case BR_RECODE_PROC_MY:
          p_param->menuID			= BR_BROWSE_MENUID_BTLVIDEO;
          break;
        case BR_RECODE_PROC_OTHER_00:
        case BR_RECODE_PROC_OTHER_01:
        case BR_RECODE_PROC_OTHER_02:
          p_param->menuID			= BR_BROWSE_MENUID_OTHER_RECORD;
          break;
        case BR_RECODE_PROC_DOWNLOAD_RANK:
          GF_ASSERT( 0 ); //�����͂��Ȃ�
          p_param->menuID			= BR_BTLVIDEO_MENUID_RANK;
          break;
        case BR_RECODE_PROC_DOWNLOAD_NUMBER:
          GF_ASSERT( 0 ); //�����͂��Ȃ�
          p_param->menuID			= BR_BTLVIDEO_MENUID_RANK;
          break;
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

    case BR_PROCID_MUSICAL_SEND:
      { 
        const BR_MUSICALSEND_PROC_PARAM *cp_musicalsend_param = cp_pre_param;
        if( cp_musicalsend_param->ret == BR_MUSICALSEND_RET_RETURN )
        { 
          p_param->fade_type = BR_FADE_TYPE_MASTERBRIGHT_AND_ALPHA;
        }
        p_param->menuID			= BR_MUSICAL_MENUID_TOP;
      }
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
  p_param->cp_saveinfo = &p_wk->p_param->p_data->rec_saveinfo;
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

  //�u�o�g���r�f�I�𑗂�v����̖߂��
  //�͂��A�������ł͂Ȃ��A���̑O�ɖ߂�
  {
    u16 stack_num = p_param->p_btn_recovery->stack_num - 1;
    if( p_param->p_btn_recovery->stack[ stack_num ].menuID == BR_MENUID_BVSEND_YESNO )
    { 
      p_param->p_btn_recovery->stack_num--;
    }
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
static void BR_RECORD_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{	
	BR_RECORD_PROC_PARAM			*p_param	= p_param_adrs;
	BR_CORE_WORK							*p_wk			= p_wk_adrs;

	//���j���[�ȊO���痈�Ȃ�

  p_param->p_res			  = p_wk->p_res;
  p_param->p_fade       = p_wk->p_fade;
  p_param->p_procsys	  = p_wk->p_procsys;
  p_param->p_unit			  = BR_GRAPHIC_GetClunit( p_wk->p_graphic );
  p_param->p_net        = p_wk->p_net;
  p_param->p_gamedata   = p_wk->p_param->p_param->p_gamedata;
  p_param->p_record     = &p_wk->p_param->p_data->record;
  p_param->is_return    = FALSE;
  p_param->cp_rec_saveinfo  = &p_wk->p_param->p_data->rec_saveinfo;
  p_param->cp_is_recplay_finish = &p_wk->p_param->p_data->is_recplay_finish;

  switch( preID )
  { 
  case BR_PROCID_MENU:
    { 
      const BR_MENU_PROC_PARAM	*cp_menu_param	= cp_pre_param;
      p_param->mode				= cp_menu_param->next_mode;
      p_param->p_outline  = NULL;

      p_wk->p_param->p_data->record_mode  = p_param->mode;

      //�������փo�g���r�f�I�ǂݍ���
      {
        GAMEDATA  *p_gamedata = p_wk->p_param->p_param->p_gamedata;
        SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_gamedata );

        LOAD_RESULT result;
        BattleRec_Load( p_sv_ctrl, HEAPID_BATTLE_RECORDER_SYS, &result, p_param->mode ); 
        if( result == LOAD_RESULT_OK )
        { 
          if( p_param->mode == LOADDATA_MYREC )
          { 

            GDS_Profile_MyDataSet( BattleRec_GDSProfilePtrGet(), p_gamedata);
          }
          NAGI_Printf( "�퓬�^��ǂݍ��� %d\n", p_param->mode );
        }
        else
        { 
          GF_ASSERT(0);
        }
      }
    }
    break;

  case BR_PROCID_BV_RANK:
    { 
      const BR_BVRANK_PROC_PARAM  *cp_rank_param  = cp_pre_param;
      p_param->mode       = BR_RECODE_PROC_DOWNLOAD_RANK;
      p_param->p_outline  = &p_wk->p_param->p_data->outline;

      p_wk->p_param->p_data->record_mode  = p_param->mode;
    }
    break;

  case BR_PROCID_CODEIN:
    { 
      const BR_CODEIN_PROC_PARAM  *cp_codein_param  = cp_pre_param;
      p_param->mode         = BR_RECODE_PROC_DOWNLOAD_NUMBER;
      p_param->video_number = cp_codein_param->video_number;
      p_param->p_outline    = NULL;

      p_wk->p_param->p_data->record_mode  = p_param->mode;
    }
    break;

  case BR_PROCID_BV_SAVE:
    //�ۑ�����̖߂�
    //��UBRS��ɓǂݍ��ނ��A���̂��ƃZ�[�u�`�F�b�N�����邽��
    //BRS��͊O���Z�[�u�f�[�^�̉\��������
    { 
      const BR_BVSAVE_PROC_PARAM  *cp_bvsave_param  = cp_pre_param;
      p_param->mode				    = p_wk->p_param->p_data->record_mode;
      p_param->p_outline      = &p_wk->p_param->p_data->outline;
      p_param->video_number   = cp_bvsave_param->video_number;
    }
    break;
    
  case BR_PROC_SYS_RECOVERY_ID:
    //�퓬����̕��A��
    { 
      //�퓬����A���Ă����Ƃ������Ƃ́A�^��f�[�^��BRS��ɂ���
      p_param->mode				    = p_wk->p_param->p_data->record_mode;
      p_param->p_outline      = &p_wk->p_param->p_data->outline;
      p_param->is_return      = TRUE;
    }
    break;

  default:
    GF_ASSERT_MSG( 0, "���j���[�ƃ����L���O�A�R�[�h���́A�ۑ��A���A�ȊO����͗��Ȃ� %d", preID );
    break;
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
  p_param->p_gamedata = p_wk->p_param->p_param->p_gamedata;
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

  p_param->p_outline      = &p_wk->p_param->p_data->outline;
  p_param->p_list_pos     = &p_wk->p_param->p_data->rank_pos;

  if( preID == BR_PROCID_MENU )
  { 
    const BR_MENU_PROC_PARAM  *cp_menu_param = cp_pre_param;
    p_param->mode       = cp_menu_param->next_mode;

    p_wk->rank_mode = p_param->mode;

    //�J�[�\���ʒu�̓��R�[�h��ʂƃ����N��ʂ̊Ԃ����ێ����Ȃ��i�����ɂ̓o�g���̍s���߂���j�̂ŁA
    //�����痈���ꍇ�N���A����
    GFL_STD_MemClear( p_param->p_list_pos, sizeof(BR_LIST_POS) );
  }
  else if( preID == BR_PROCID_BV_SEARCH )
  {
    const BR_BVSEARCH_PROC_PARAM  *cp_search_param = cp_pre_param;
    p_param->mode         = BR_BVRANK_MODE_SEARCH;
    p_param->search_data  = cp_search_param->search_data;
    
    p_wk->rank_mode = p_param->mode;

    //�J�[�\���ʒu�̓��R�[�h��ʂƃ����N��ʂ̊Ԃ����ێ����Ȃ��i�����ɂ̓o�g���̍s���߂���j�̂ŁA
    //�����痈���ꍇ�N���A����
    GFL_STD_MemClear( p_param->p_list_pos, sizeof(BR_LIST_POS) );
  }
  else if( preID == BR_PROCID_RECORD )
  {
    p_param->mode         = p_wk->rank_mode;
    p_param->is_return    = TRUE;
  }
  else
  { 
    GF_ASSERT_MSG( 0, "%d", preID );
  }

	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
  p_param->p_net      = p_wk->p_net;
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
	BR_BVSEARCH_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK					  	*p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
  p_param->p_gamedata = p_wk->p_param->p_param->p_gamedata;
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
  p_param->p_net      = p_wk->p_net;
  p_param->p_gamedata = p_wk->p_param->p_param->p_gamedata;
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
static void BR_BVDELETE_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_BVDELETE_PROC_PARAM	*p_param	= p_param_adrs;
	BR_CORE_WORK						*p_wk			= p_wk_adrs;

  if( preID == BR_PROCID_MENU )
  { 
    const BR_MENU_PROC_PARAM	*cp_menu_param	= cp_pre_param;
    p_param->mode				= cp_menu_param->next_mode;
    p_param->p_res			= p_wk->p_res;
    p_param->p_fade     = p_wk->p_fade;
    p_param->p_procsys	= p_wk->p_procsys;
    p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
    p_param->p_gamedata = p_wk->p_param->p_param->p_gamedata;
    p_param->cp_rec_saveinfo = &p_wk->p_param->p_data->rec_saveinfo;
  }
  else
  { 
    GF_ASSERT(0);
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
static void BR_BVDELETE_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 
  BR_BVDELETE_PROC_PARAM	*p_param	= p_param_adrs;
	BR_CORE_WORK						*p_wk			= p_wk_adrs;

  //�����Ă�����A�Z�[�u�f�[�^�������[�h����
  if( p_param->is_delete )
  { 
    Br_Core_CheckSaveData( &p_wk->p_param->p_data->rec_saveinfo,
      FALSE, p_wk->p_param->p_param->p_gamedata, HEAPID_BATTLE_RECORDER_CORE );
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
static void BR_BVSAVE_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_BVSAVE_PROC_PARAM	    *p_param	= p_param_adrs;
	BR_CORE_WORK						  *p_wk			= p_wk_adrs;
  const BR_RECORD_PROC_PARAM	*cp_record_param	= cp_pre_param;

  GF_ASSERT( preID == BR_PROCID_RECORD );

  p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
  p_param->p_net      = p_wk->p_net;
  p_param->p_procsys	= p_wk->p_procsys;
  p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
  p_param->video_number = cp_record_param->video_number;
  p_param->p_gamedata = p_wk->p_param->p_param->p_gamedata;
  p_param->cp_rec_saveinfo = &p_wk->p_param->p_data->rec_saveinfo;
  p_param->is_secure  = cp_record_param->is_secure;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���b�N������֐�
 *
 *	@param	void *p_param_adrs	�������[�N
 *	@param	*p_wk_adrs					���C�����[�N
 */
//-----------------------------------------------------------------------------
static void BR_BVSAVE_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 
  BR_BVSAVE_PROC_PARAM	*p_param	= p_param_adrs;
	BR_CORE_WORK						*p_wk			= p_wk_adrs;

  //�ۑ����Ă�����A�Z�[�u�f�[�^�������[�h����
  if( p_param->is_save )
  { 
    Br_Core_CheckSaveData( &p_wk->p_param->p_data->rec_saveinfo,
      FALSE, p_wk->p_param->p_param->p_gamedata, HEAPID_BATTLE_RECORDER_CORE );
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
static void BR_MUSICALLOOK_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_MUSICALLOOK_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						    *p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
  p_param->p_fade     = p_wk->p_fade;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_graphic	= p_wk->p_graphic;
  p_param->p_net      = p_wk->p_net;
  p_param->p_sidebar  = p_wk->p_sidebar;
  p_param->p_gamedata = p_wk->p_param->p_param->p_gamedata;
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
  p_param->p_net      = p_wk->p_net;
  p_param->p_sidebar  = p_wk->p_sidebar;
  p_param->p_gamedata = p_wk->p_param->p_param->p_gamedata;
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
//=============================================================================
/**
 *    ���̑�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �O���Z�[�u�f�[�^���`�F�b�N
 *	        �i�O���Z�[�u�f�[�^���`�F�b�N����Ƃ���BRS���㏑�����Ă��܂����Ƃɒ��Ӂj
 *
 *	@param	BR_SAVE_INFO *p_saveinfo  �Z�[�u�f�[�^�󋵃��[�N
 *	@param  is_onece                  TRUE�ň�x�`�F�b�N���Ă���ƍs��Ȃ�FALSE�ł��ł��s�Ȃ�
 *	@param	*p_gamedata               �Q�[���f�[�^
 *	@param	heapID                    �e���|�����q�[�vID
 */
//-----------------------------------------------------------------------------
static void Br_Core_CheckSaveData( BR_SAVE_INFO *p_saveinfo, BOOL is_onece, GAMEDATA *p_gamedata, HEAPID heapID )
{ 
  BOOL  is_check  = TRUE;

  if( is_onece )
  { 
    is_check  = !p_saveinfo->is_check;
  }

  if( is_check )
  { 
	  int i;
	  SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_gamedata );
    GDS_PROFILE_PTR p_profile;
    LOAD_RESULT result;

    //������x�`�F�b�N���Ă�������
    if( p_saveinfo->is_check )
    { 
      for( i = 0; i < BR_SAVEDATA_NUM; i++ )
      { 
        if( p_saveinfo->p_name[i] )
        { 
          GFL_STR_DeleteBuffer( p_saveinfo->p_name[i] );
          p_saveinfo->p_name[i] = NULL;
        }
      }
    }

    //���܂ł̏����N���A
    GFL_STD_MemClear( p_saveinfo, sizeof(BR_SAVE_INFO) );
	
	  //�o�g���r�f�I�̃Z�[�u�󋵂��擾
	  for( i = 0; i < BR_SAVEDATA_NUM; i++ )
	  {	
	    BattleRec_Load( p_sv, GFL_HEAP_LOWID( heapID ), &result, i ); 
	    if( result == LOAD_RESULT_OK )
	    { 
	      NAGI_Printf( "�퓬�^��ǂݍ��� %d\n", i );
	      p_profile = BattleRec_GDSProfilePtrGet();

        if( i == 0 )
        { 
          //�����̘^��f�[�^�͎����̏�����ɍŐV�ɂ��Ă������ߐݒ肷��
          GDS_Profile_MyDataSet(p_profile, p_gamedata );
        }


        //�ݒ�
	      p_saveinfo->is_valid[i] = TRUE;

        //�����br_main.c�ōs���Ă��܂�
        p_saveinfo->p_name[i]   = GDS_Profile_CreateNameString( p_profile, HEAPID_BATTLE_RECORDER_SYS );
        DEBUG_STRBUF_Print( p_saveinfo->p_name[i] );

        //���ʎ擾
	      p_saveinfo->sex[i]      = GDS_Profile_GetSex( p_profile );

        //�r�f�I�ԍ��擾
        p_saveinfo->video_number[i]  = RecHeader_ParamGet( BattleRec_HeaderPtrGet(), RECHEAD_IDX_DATA_NUMBER, 0 );
	    }
	  }
	
	  //�~���[�W�J���̃Z�[�u�󋵂��擾
	  {
	    MUSICAL_SAVE* p_musical = MUSICAL_SAVE_GetMusicalSave( GAMEDATA_GetSaveControlWork(p_gamedata) );
      p_saveinfo->is_musical_valid  = MUSICAL_SAVE_IsValidMusicalShotData( p_musical );
    }
	
	  //�`�F�b�N�t���O�𗧂Ă�
	  p_saveinfo->is_check  = TRUE;
  }
}
