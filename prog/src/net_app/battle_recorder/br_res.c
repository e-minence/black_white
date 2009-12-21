//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_res.c
 *	@brief	�o�g�����R�[�_�[���\�[�X�Ǘ�
 *	@author	Toru=Nagihashi
 *	@data		2009.11.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID

//�A�[�J�C�u
#include "arc_def.h"
#include "battle_recorder_gra.naix"
#include "message.naix"

//�t�H���g
#include "font/font.naix"

//�����̃��W���[��
#include "br_inner.h"

//�O���Q��
#include "br_res.h"
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
///	���\�[�X���[�N
//=====================================
struct _BR_RES_WORK
{	
	u32							obj_common_plt[ CLSYS_DRAW_MAX ];
	BR_RES_OBJ_DATA	obj[BR_RES_OBJ_MAX];
	BOOL						obj_flag[BR_RES_OBJ_MAX];
	GFL_FONT				*p_font;
	GFL_MSGDATA			*p_msg;
  WORDSET         *p_word;
};

//=============================================================================
/**
 *					���\�[�X�Ǘ�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�Ǘ�������
 *
 *	@param	HEAPID heapID		�q�[�vID
 *
 *	@return	���\�[�X���[�N
 */
//-----------------------------------------------------------------------------
BR_RES_WORK *BR_RES_Init( HEAPID heapID )
{	
	BR_RES_WORK *p_wk;

	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(BR_RES_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(BR_RES_WORK) );

	//�ŏ��ɓǂݍ���ł������\�[�X
	p_wk->p_font	= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
	p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_battle_rec_dat, heapID );
  p_wk->p_word  = WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_COUNTRY_BUFLEN, heapID );

  BR_RES_LoadCommon( p_wk, CLSYS_DRAW_MAIN, heapID );
  BR_RES_LoadCommon( p_wk, CLSYS_DRAW_SUB, heapID );

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�Ǘ��j��
 *
 *	@param	BR_RES_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_RES_Exit( BR_RES_WORK *p_wk )
{	
	//����Y��`�F�b�N
	{	
		int i;
		for( i = 0; i < BR_RES_OBJ_MAX; i++ )
		{	
			GF_ASSERT_MSG( p_wk->obj_flag[i] == FALSE, "����Y�� %d\n", i );
		}
	}

  BR_RES_UnLoadCommon( p_wk, CLSYS_DRAW_MAIN );
  BR_RES_UnLoadCommon( p_wk, CLSYS_DRAW_SUB );

	//���ʂ��̑����\�[�X�j��
  WORDSET_Delete( p_wk->p_word );
	GFL_MSG_Delete( p_wk->p_msg );
	GFL_FONT_Delete( p_wk->p_font );

	GFL_HEAP_FreeMemory( p_wk );
}

//=============================================================================
/**
 *					���\�[�X�ǂݍ���
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG�ǂݍ���	
 *
 *	@param	BR_RES_WORK *p_wk	���[�N
 *	@param	bgID						BG��ID
 */
//-----------------------------------------------------------------------------
void BR_RES_LoadBG( BR_RES_WORK *p_wk, BR_RES_BGID bgID, HEAPID heapID )
{	
	ARCHANDLE * p_handle  = GFL_ARC_OpenDataHandle( ARCID_BATTLE_RECORDER_GRA, heapID );

	switch( bgID )
	{	
	case BR_RES_BG_START_M:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_browse_bg_NCGR,
        BG_FRAME_M_BACK, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_browse_bg0u_NSCR,
				BG_FRAME_M_BACK, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_START_S:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_browse_bg_NCGR,
        BG_FRAME_S_BACK, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_browse_bg0d_NSCR,
				BG_FRAME_S_BACK, 0, 0, FALSE, heapID );
		break;

  case BR_RES_BG_TEXT_M:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_win_NCGR,
        BG_FRAME_M_TEXT, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_win2_NSCR,
				BG_FRAME_M_TEXT, 0, 0, FALSE, heapID );
    break;

	case BR_RES_BG_RECORD_S:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_S_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_video_d2_NSCR,
				BG_FRAME_S_WIN, 0, 0, FALSE, heapID );
		break;

	case BR_RES_BG_RECORD_M_BTL_SINGLE:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_bg1ua_data_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
		break;

	case BR_RES_BG_RECORD_M_BTL_DOUBLE:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_bg1ub_data_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
		break;

	case BR_RES_BG_RECORD_M_PROFILE:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_bg1uc_data_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
		break;

  case BR_RES_BG_SUBWAY_S:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_S_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_subway_d_NSCR,
				BG_FRAME_S_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_SUBWAY_M_SINGLE:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_subway_u1_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_SUBWAY_M_DOUBLE:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_subway_u2_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_SUBWAY_M_MULTI:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_subway_u3_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_SUBWAY_M_WIFI:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_subway_u4_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
    break;
    
  case BR_RES_BG_RNDMATCH_M_NONE:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_win_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_win2_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_SUBWAY_M_NONE:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_subway_u0_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_RNDMATCH_S:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_S_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_rndmatch_d_NSCR,
				BG_FRAME_S_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_RNDMATCH_M_FREE:
     GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_rndmatch_u1_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_RNDMATCH_M_RATE:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_rndmatch_u2_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_RNDMATCH_M_RND:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_rndmatch_u3_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_POKESEARCH_S_HEAD:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_S_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_find_d2_NSCR,
				BG_FRAME_S_WIN, 0, 0, FALSE, heapID );
		break;

  case BR_RES_BG_POKESEARCH_S_LIST:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_S_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_find_d4_NSCR,
				BG_FRAME_S_WIN, 0, 0, FALSE, heapID );
    break;
    
  case BR_RES_BG_PHOTO_S:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_S_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_photo_d_NSCR,
				BG_FRAME_S_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_PHOTO_SEND_S:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_S_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_photo_d2_NSCR,
				BG_FRAME_S_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_BVRANK:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_video_u_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_S_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_video_d1_NSCR,
				BG_FRAME_S_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_BVSEARCH_M:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_M_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_find_u_NSCR,
				BG_FRAME_M_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_BVSEARCH_MENU_S:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_S_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_find_d3_NSCR,
				BG_FRAME_S_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_BVSEARCH_PLACE_S:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_data_NCGR,
        BG_FRAME_S_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_find_d1_NSCR,
				BG_FRAME_S_WIN, 0, 0, FALSE, heapID );
    break;

  case BR_RES_BG_CODEIN_NUMBER_S:
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_battle_recorder_gra_batt_rec_browse_bg_NCGR,
        BG_FRAME_S_WIN, 0, 0, FALSE, heapID );

    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_battle_recorder_gra_batt_rec_nam_NSCR,
				BG_FRAME_S_WIN, 0, 0, FALSE, heapID );
    break;
	}

	GFL_ARC_CloseDataHandle( p_handle );
}
//----------------------------------------------------------------------------
/**
 *	@brief	BG�j��
 *
 *	@param	BR_RES_WORK *p_wk	���[�N
 *	@param	bgID							BG��ID
 */
//-----------------------------------------------------------------------------
void BR_RES_UnLoadBG( BR_RES_WORK *p_wk, BR_RES_BGID bgID )
{	
	switch( bgID )
	{	
  case BR_RES_BG_BVRANK:
		GFL_BG_ClearScreen( BG_FRAME_M_WIN );
		GFL_BG_LoadScreenReq( BG_FRAME_M_WIN );
		GFL_BG_ClearScreen( BG_FRAME_S_WIN );
		GFL_BG_LoadScreenReq( BG_FRAME_S_WIN );
    break;

	case BR_RES_BG_START_M:
		GFL_BG_ClearScreen( BG_FRAME_M_BACK );
		GFL_BG_LoadScreenReq( BG_FRAME_M_BACK );
		break;

	case BR_RES_BG_START_S:
		GFL_BG_ClearScreen( BG_FRAME_S_BACK );
		GFL_BG_LoadScreenReq( BG_FRAME_S_BACK );
		break;

  case BR_RES_BG_TEXT_M:
		GFL_BG_ClearScreen( BG_FRAME_M_TEXT );
		GFL_BG_LoadScreenReq( BG_FRAME_M_TEXT );
    break;

  case BR_RES_BG_CODEIN_NUMBER_S:
    /* fallthrough */
  case BR_RES_BG_PHOTO_S:
    /* fallthrough */
  case BR_RES_BG_PHOTO_SEND_S:
    /* fallthrough */
  case BR_RES_BG_BVSEARCH_MENU_S:
    /* fallthrough */
  case BR_RES_BG_BVSEARCH_PLACE_S:
    /* fallthrough */
  case BR_RES_BG_POKESEARCH_S_HEAD:
		/* fallthrough */
  case BR_RES_BG_POKESEARCH_S_LIST:
		/* fallthrough */
  case BR_RES_BG_RNDMATCH_S:
		/* fallthrough */
  case BR_RES_BG_SUBWAY_S:
		/* fallthrough */
	case BR_RES_BG_RECORD_S:
		GFL_BG_ClearScreen( BG_FRAME_S_WIN );
		GFL_BG_LoadScreenReq( BG_FRAME_S_WIN );
		break;

  case BR_RES_BG_BVSEARCH_M:
    /* fallthrough */
  case BR_RES_BG_RNDMATCH_M_RND:
		/* fallthrough */
  case BR_RES_BG_RNDMATCH_M_RATE:
		/* fallthrough */
  case BR_RES_BG_RNDMATCH_M_FREE:
		/* fallthrough */
  case BR_RES_BG_RNDMATCH_M_NONE:
		/* fallthrough */
  case BR_RES_BG_SUBWAY_M_SINGLE:
		/* fallthrough */
  case BR_RES_BG_SUBWAY_M_DOUBLE:
		/* fallthrough */
  case BR_RES_BG_SUBWAY_M_MULTI:
		/* fallthrough */
  case BR_RES_BG_SUBWAY_M_WIFI:
		/* fallthrough */
	case BR_RES_BG_RECORD_M_BTL_SINGLE:
		/* fallthrough */
	case BR_RES_BG_RECORD_M_BTL_DOUBLE:
		/* fallthrough */
	case BR_RES_BG_RECORD_M_PROFILE:
		GFL_BG_ClearScreen( BG_FRAME_M_WIN );
		GFL_BG_LoadScreenReq( BG_FRAME_M_WIN );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�ǂݍ���
 *
 *	@param	BR_RES_WORK *p_wk	���[�N
 *	@param	objID							OBJID
 *	@param	heapID						�q�[�vID
 */
//-----------------------------------------------------------------------------
void BR_RES_LoadOBJ( BR_RES_WORK *p_wk, BR_RES_OBJID objID, HEAPID heapID )
{	
	BR_RES_OBJ_DATA *p_data	= &p_wk->obj[ objID ];

	GF_ASSERT( p_wk->obj_flag[ objID ] == FALSE );

	switch( objID )
	{	
  case BR_RES_OBJ_SIDEBAR_M:
		{	
			ARCHANDLE *p_handle;

			p_handle  = GFL_ARC_OpenDataHandle( ARCID_BATTLE_RECORDER_GRA, heapID );
			p_data->ncl	= p_wk->obj_common_plt[ CLSYS_DRAW_MAIN ];
			p_data->ncg	= GFL_CLGRP_CGR_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_browse_line_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );
			p_data->nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_line_64k_NCER, NARC_battle_recorder_gra_batt_rec_line_64k_NANR, heapID );

			GFL_ARC_CloseDataHandle( p_handle );
		}
    break;
  case BR_RES_OBJ_SIDEBAR_S:
    {	
			ARCHANDLE *p_handle;

			p_handle  = GFL_ARC_OpenDataHandle( ARCID_BATTLE_RECORDER_GRA, heapID );
			p_data->ncl	= p_wk->obj_common_plt[ CLSYS_DRAW_SUB ];
			p_data->ncg	= GFL_CLGRP_CGR_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_browse_line_NCGR, FALSE, CLSYS_DRAW_SUB, heapID );
			p_data->nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_line_64k_NCER, NARC_battle_recorder_gra_batt_rec_line_64k_NANR, heapID );

			GFL_ARC_CloseDataHandle( p_handle );
		}
    break;
	case BR_RES_OBJ_BROWSE_BTN_M:
		{	
			ARCHANDLE *p_handle;

			p_handle  = GFL_ARC_OpenDataHandle( ARCID_BATTLE_RECORDER_GRA, heapID );
			p_data->ncl	= p_wk->obj_common_plt[ CLSYS_DRAW_MAIN ];
			p_data->ncg	= GFL_CLGRP_CGR_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_browse_tag_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );
			p_data->nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_tag_browse_NCER, NARC_battle_recorder_gra_batt_rec_tag_browse_NANR, heapID );

			GFL_ARC_CloseDataHandle( p_handle );
		}
		break;

	case BR_RES_OBJ_BROWSE_BTN_S:
		{	
			ARCHANDLE *p_handle;

			p_handle  = GFL_ARC_OpenDataHandle( ARCID_BATTLE_RECORDER_GRA, heapID );
			p_data->ncl	= p_wk->obj_common_plt[ CLSYS_DRAW_SUB ];
			p_data->ncg	= GFL_CLGRP_CGR_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_browse_tag_NCGR, FALSE, CLSYS_DRAW_SUB, heapID );
			p_data->nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_tag_browse_NCER, NARC_battle_recorder_gra_batt_rec_tag_browse_NANR, heapID );

			GFL_ARC_CloseDataHandle( p_handle );
		}
		break;

	case BR_RES_OBJ_SHORT_BTN_S:
		{	
			ARCHANDLE *p_handle;

			p_handle  = GFL_ARC_OpenDataHandle( ARCID_BATTLE_RECORDER_GRA, heapID );
			p_data->ncl	= p_wk->obj_common_plt[ CLSYS_DRAW_SUB ];
			p_data->ncg	= GFL_CLGRP_CGR_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_gds_tag2_NCGR, FALSE, CLSYS_DRAW_SUB, heapID );
			p_data->nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_tag2_gds_64k_NCER, NARC_battle_recorder_gra_batt_rec_tag2_gds_64k_NANR, heapID );

			GFL_ARC_CloseDataHandle( p_handle );
		}
		break;

  case BR_RES_OBJ_ALLOW_S:
		{	
			ARCHANDLE *p_handle;

			p_handle  = GFL_ARC_OpenDataHandle( ARCID_BATTLE_RECORDER_GRA, heapID );
			p_data->ncl	= p_wk->obj_common_plt[ CLSYS_DRAW_SUB ];
			p_data->ncg	= GFL_CLGRP_CGR_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_cursor_NCGR, FALSE, CLSYS_DRAW_SUB, heapID );
			p_data->nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_cursor_NCER, NARC_battle_recorder_gra_batt_rec_cursor_NANR, heapID );

			GFL_ARC_CloseDataHandle( p_handle );
		}
    break;

  case BR_RES_OBJ_ALLOW_M:
		{	
			ARCHANDLE *p_handle;

			p_handle  = GFL_ARC_OpenDataHandle( ARCID_BATTLE_RECORDER_GRA, heapID );
			p_data->ncl	= p_wk->obj_common_plt[ CLSYS_DRAW_MAIN ];
			p_data->ncg	= GFL_CLGRP_CGR_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_cursor_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );
			p_data->nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_cursor_NCER, NARC_battle_recorder_gra_batt_rec_cursor_NANR, heapID );

			GFL_ARC_CloseDataHandle( p_handle );
		}
    break;

  case BR_RES_OBJ_NUM_S:
		{	
			ARCHANDLE *p_handle;

			p_handle  = GFL_ARC_OpenDataHandle( ARCID_BATTLE_RECORDER_GRA, heapID );
			p_data->ncl	= GFL_CLGRP_PLTT_RegisterEx( p_handle, 
          NARC_battle_recorder_gra_batt_rec_nam_NCLR,
          CLSYS_DRAW_SUB, PLT_OBJ_S_SPECIAL*0x20, 0,1,heapID );
			p_data->ncg	= GFL_CLGRP_CGR_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_nam_obj_NCGR, FALSE, CLSYS_DRAW_SUB, heapID );
			p_data->nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
					NARC_battle_recorder_gra_batt_rec_nam_obj_NCER, NARC_battle_recorder_gra_batt_rec_nam_obj_NANR, heapID );

			GFL_ARC_CloseDataHandle( p_handle );
		}
    break;

  case BR_RES_OBJ_NUM_CURSOR_S:
		{	
			ARCHANDLE *p_handle;

			p_handle  = GFL_ARC_OpenDataHandle( ARCID_BATTLE_RECORDER_GRA, heapID );
			p_data->ncl	= GFL_CLGRP_PLTT_RegisterEx( p_handle, 
          NARC_battle_recorder_gra_battle_rec_code_cur_NCLR,
          CLSYS_DRAW_SUB, (PLT_OBJ_S_SPECIAL+1)*0x20, 0, 1, heapID );
			p_data->ncg	= GFL_CLGRP_CGR_Register( p_handle,
					NARC_battle_recorder_gra_battle_rec_code_cur_NCGR, FALSE, CLSYS_DRAW_SUB, heapID );
			p_data->nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
					NARC_battle_recorder_gra_battle_rec_code_cur_NCER, NARC_battle_recorder_gra_battle_rec_code_cur_NANR, heapID );

			GFL_ARC_CloseDataHandle( p_handle );
		}
    break;
	}


	p_wk->obj_flag[ objID ] = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�j��
 *
 *	@param	BR_RES_WORK *p_wk		���[�N
 *	@param	objID								OBJID
 */
//-----------------------------------------------------------------------------
void BR_RES_UnLoadOBJ( BR_RES_WORK *p_wk, BR_RES_OBJID objID )
{
	BR_RES_OBJ_DATA *p_data	= &p_wk->obj[ objID ];
	GF_ASSERT( p_wk->obj_flag[ objID ] == TRUE );

	switch( objID )
	{	
  case BR_RES_OBJ_NUM_CURSOR_S:
    /* fallthrough */
  case BR_RES_OBJ_NUM_S:
    GFL_CLGRP_PLTT_Release( p_data->ncl );
    GFL_CLGRP_CGR_Release( p_data->ncg );
    GFL_CLGRP_CELLANIM_Release( p_data->nce );
    break;
  case BR_RES_OBJ_ALLOW_M:
    /* fallthrough */
  case BR_RES_OBJ_ALLOW_S:
    /* fallthrough */
	case BR_RES_OBJ_BROWSE_BTN_M:
		/* fallthrough */
	case BR_RES_OBJ_BROWSE_BTN_S:
		/* fallthrough */
	case BR_RES_OBJ_SHORT_BTN_S:
    GFL_CLGRP_CGR_Release( p_data->ncg );
    GFL_CLGRP_CELLANIM_Release( p_data->nce );
		break;

	}

	p_wk->obj_flag[ objID ] = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�p���\�[�X�󂯎��
 *
 *	@param	const BR_RES_WORK *cp_wk	���[�N
 *	@param	objID											OBJID
 *	@param	*p_data										�󂯎��p���[�N
 *
 *	@retval TRUE �󂯎�萬��	FALSE�ǂݍ��܂�Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BR_RES_GetOBJRes( const BR_RES_WORK *cp_wk, BR_RES_OBJID objID, BR_RES_OBJ_DATA *p_data )
{	
	if( cp_wk->obj_flag[ objID ] )
	{	
		*p_data	= cp_wk->obj[ objID ];
	}

	return cp_wk->obj_flag[ objID ];
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���ʃ��\�[�X�ǂݍ���
 *
 *	@param	BR_RES_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_RES_LoadCommon( BR_RES_WORK *p_wk, CLSYS_DRAW_TYPE type, HEAPID heapID )
{ 
  //���ʃO���t�B�b�N
  {	
		ARCHANDLE * p_handle  = GFL_ARC_OpenDataHandle( ARCID_BATTLE_RECORDER_GRA, heapID );

    //����OBJ�p���b�g
    { 
      u32 plt;
      if( type == CLSYS_DRAW_MAIN )
      { 
        plt = PLT_OBJ_M_COMMON;
      }
      else
      { 
        plt = PLT_OBJ_S_COMMON;
      }
      p_wk->obj_common_plt[ type ]	= GFL_CLGRP_PLTT_Register( p_handle, 
	//		NARC_battle_recorder_gra_batt_rec_browse_obj_NCLR, 
          NARC_battle_recorder_gra_batt_rec_gds_obj_NCLR,
          type, plt*0x20, heapID );
    }


    //����BG�p���b�g
    if( type == CLSYS_DRAW_MAIN )
    { 
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, 
          //NARC_battle_recorder_gra_batt_rec_browse_bg_NCLR,
          NARC_battle_recorder_gra_batt_rec_gds_bg_NCLR,
          PALTYPE_MAIN_BG, PLT_BG_M_COMMON*0x20, 0, heapID );

      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, 
        //NARC_battle_recorder_gra_batt_rec_font_NCLR,
        NARC_battle_recorder_gra_batt_rec_gds_font_NCLR,
				PALTYPE_MAIN_BG, PLT_BG_M_FONT*0x20, 0x20, heapID );
    }
    else
    { 

      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, 
       // NARC_battle_recorder_gra_batt_rec_browse_bg_NCLR,
        NARC_battle_recorder_gra_batt_rec_gds_bg_NCLR,
				PALTYPE_SUB_BG, PLT_BG_S_COMMON*0x20, 0, heapID );

      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, 
        //NARC_battle_recorder_gra_batt_rec_font_NCLR,
        NARC_battle_recorder_gra_batt_rec_gds_font_NCLR,
				PALTYPE_SUB_BG, PLT_BG_S_FONT*0x20, 0x20, heapID );

    }

		GFL_ARC_CloseDataHandle( p_handle );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���ʃ��\�[�X�j��
 *
 *	@param	BR_RES_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_RES_UnLoadCommon( BR_RES_WORK *p_wk, CLSYS_DRAW_TYPE type )
{ 
	//���ʃO���t�B�b�N�j��
	GFL_CLGRP_PLTT_Release( p_wk->obj_common_plt[ type ] );
}
//=============================================================================
/**
 *				���̑����\�[�X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�t�H���g�p���\�[�X�󂯎��
 *
 *	@param	const BR_RES_WORK *cp_wk	���[�N
 *
 *	@return	�t�H���g
 */
//-----------------------------------------------------------------------------
GFL_FONT * BR_RES_GetFont( const BR_RES_WORK *cp_wk )
{	
	return cp_wk->p_font;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�t�H���g�p���\�[�X�󂯎��
 *
 *	@param	const BR_RES_WORK *cp_wk	���[�N
 *
 *	@return	�t�H���g
 */
//-----------------------------------------------------------------------------
GFL_MSGDATA * BR_RES_GetMsgData( const BR_RES_WORK *cp_wk )
{	
	return cp_wk->p_msg;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �P��o�^�����Ƃ�
 *
 *	@param	const BR_RES_WORK *cp_wk  ���[�N
 *
 *	@return �P��o�^
 */
//-----------------------------------------------------------------------------
WORDSET * BR_RES_GetWordSet( const BR_RES_WORK *cp_wk )
{ 
  return cp_wk->p_word;
}
