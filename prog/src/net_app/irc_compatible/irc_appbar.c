//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_appbar.c
 *	@brief	�ԊO���`�F�b�N�p����ʃo�[
 *	@author	Toru=Nagihashi
 *	@data		2009.07.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>

//archive
#include "arc_def.h"
#include "message.naix"
#include "msg/msg_irc_compatible.h"
#include "app/app_taskmenu.h"
#include "irccompatible_gra.naix"
#include "sound/pm_sndsys.h"

//mine
#include "net_app/irc_appbar.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	APPBAR
//=====================================
//���\�[�X
enum
{	
	APPBAR_RES_COMMON_PLT,
	APPBAR_RES_COMMON_CHR,
	APPBAR_RES_COMMON_CEL,

	APPBAR_RES_BAR_PLT,
	APPBAR_RES_BAR_CHR,
	APPBAR_RES_BAR_CEL,

	APPBAR_RES_MAX
};

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	����ʃo�[(�A�v���P�[�V�����o�[�Ə���ɖ���)
//=====================================
struct _APPBAR_WORK
{
  APP_TASKMENU_WIN_WORK   *p_win;
  APP_TASKMENU_RES        *p_menu_res;
	GFL_CLWK	*p_clwk[APPBAR_ICON_MAX];
	u32				res[APPBAR_RES_MAX];
	u32				bg_frm;
	GFL_ARCUTIL_TRANSINFO				chr_pos;
	s32				trg;
	s32				cont;
	u32				tbl_len;
  s32       seq;
  APPBAR_BUTTON_TYPE  btn_type;
  HEAPID              heapID;
  BOOL      is_use;
};


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//���\�[�X
static void AppBar_LoadResource( APPBAR_WORK *p_wk, CLSYS_DRAW_TYPE	clsys_draw_type, PALTYPE bgplttype, APP_COMMON_MAPPING mapping, u8 bg_plt, u8 obj_plt, HEAPID heapID );
static void AppBar_UnLoadResource( APPBAR_WORK *p_wk );
//obj
static void AppBar_CreateObj( APPBAR_WORK *p_wk, GFL_CLUNIT* p_unit, APPBAR_OPTION_MASK mask, CLSYS_DEFREND_TYPE	clsys_def_type, u8 bg_pri, HEAPID heapID );
static void AppBar_DeleteObj( APPBAR_WORK *p_wk );
//etc
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h,  u8 plt, BOOL compressedFlag, HEAPID heapID );

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================


//=============================================================================
/**
 *					GLOBAL
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief	�������g���Łi�A�C�R���ʒu�w��Łj
 *
 *	@param	const APPBAR_SETUP *cp_setup_tbl	�ݒ�e�[�u��
 *	@param	tbl_len	�e�[�u����
 *	@param	p_unit	OBJ�ݒ�UNIT
 *	@param	bar_frm	�g�pBG�ԍ�	�i�����ɏ㉺��ʂ𒲂ׂĂ��܂��j
 *	@param	bg_plt	�g�pBG�pPLTNUM	
 *	@param	obj_plt	�g�pOBJ�pPLTNUM
 *	@param	mapping	OBJ�}�b�s���O���[�h
 *	@param	heapID	�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
APPBAR_WORK * APPBAR_Init( APPBAR_OPTION_MASK mask, GFL_CLUNIT* p_unit, u8 bar_frm, u8 bg_plt, u8 obj_plt, APP_COMMON_MAPPING mapping, GFL_FONT* p_font, PRINT_QUE* p_que, HEAPID heapID )
{	
	APPBAR_WORK	*p_wk;

	GF_ASSERT( p_unit );

	//���[�N�쐬�A������
	{	
		u32 size;

		size	= sizeof(APPBAR_WORK);
		p_wk	= GFL_HEAP_AllocMemory( heapID, size );
		GFL_STD_MemClear( p_wk, size );
		p_wk->bg_frm	= bar_frm;
		p_wk->trg			= APPBAR_SELECT_NONE;
		p_wk->cont		= APPBAR_SELECT_NONE;
    p_wk->btn_type= APPBAR_BUTTON_TYPE_RETURN;
    p_wk->heapID  = heapID;
    p_wk->is_use  = TRUE;
	}

	{	
		CLSYS_DRAW_TYPE			clsys_draw_type;
		CLSYS_DEFREND_TYPE	clsys_def_type;
		PALTYPE							bgplttype;

		//OBJ�ǂݍ��ޏꏊ���`�F�b�N
		if( bar_frm >= GFL_BG_FRAME0_S )
		{	
			clsys_draw_type	= CLSYS_DRAW_SUB;
			clsys_def_type	= CLSYS_DEFREND_SUB;
			bgplttype				= PALTYPE_SUB_BG;
		}
		else
		{	
			clsys_draw_type	= CLSYS_DRAW_MAIN;
			clsys_def_type	= CLSYS_DEFREND_MAIN;
			bgplttype				= PALTYPE_MAIN_BG;
		}

		//���\�[�X�ǂݍ���
		AppBar_LoadResource( p_wk, clsys_draw_type, bgplttype, mapping, bg_plt, obj_plt, heapID );

    //taskmenu�p���\�[�X�ǂݍ���
    p_wk->p_menu_res  = APP_TASKMENU_RES_Create( bar_frm, bg_plt, p_font, p_que, heapID );

		//CLWK�ǂݍ���
		AppBar_CreateObj( p_wk, p_unit, mask, clsys_def_type, GFL_BG_GetPriority(bar_frm), heapID );

    //�E�B���h�E�ǂݍ���
    {
      GFL_MSGDATA *p_msg;
      APP_TASKMENU_ITEMWORK item;

      p_msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_irc_compatible_dat, heapID );

      GFL_STD_MemClear( &item, sizeof(APP_TASKMENU_ITEMWORK) );
      item.msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
      item.str      = GFL_MSG_CreateString( p_msg, COMPATI_BTN_000 );
      item.type     = APP_TASKMENU_WIN_TYPE_RETURN;

      p_wk->p_win       = APP_TASKMENU_WIN_Create( p_wk->p_menu_res, &item, 32-APP_TASKMENU_PLATE_WIDTH, 21, APP_TASKMENU_PLATE_WIDTH, heapID );

      GFL_STR_DeleteBuffer( item.str );
      GFL_MSG_Delete( p_msg );
    }
	}

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	�j��
 *
 *	@param	APPBAR_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
void APPBAR_Exit( APPBAR_WORK *p_wk )
{	
  //�E�B���h�E�j��
  APP_TASKMENU_WIN_Delete( p_wk->p_win );

	//CLWK
	AppBar_DeleteObj( p_wk );

  //taskmenu�p���\�[�X�ǂݍ���
  APP_TASKMENU_RES_Delete( p_wk->p_menu_res );

	//���\�[�X�j��
	AppBar_UnLoadResource( p_wk );
	
	//�j��
	GFL_HEAP_FreeMemory( p_wk );

}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	���C������
 *
 *	@param	APPBAR_WORK *p_wk			���[�N
 *
 */
//-----------------------------------------------------------------------------
void APPBAR_Main( APPBAR_WORK *p_wk )
{
  enum
  { 
    SEQ_TOUCH,
    SEQ_ANM,
    SEQ_END,
  };

  if( !p_wk->is_use )
  { 
    return;
  }

  switch( p_wk->seq )
  {
  case SEQ_TOUCH:
    p_wk->trg = APPBAR_SELECT_NONE;
    if( APP_TASKMENU_WIN_IsTrg( p_wk->p_win ) )
    { 
			PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );
      APP_TASKMENU_WIN_SetDecide( p_wk->p_win, TRUE );
      p_wk->seq = SEQ_ANM;
    }
    break;
  case SEQ_ANM:
    if( APP_TASKMENU_WIN_IsFinish( p_wk->p_win ) )
    { 
      APP_TASKMENU_WIN_ResetDecide( p_wk->p_win );
      APP_TASKMENU_WIN_SetActive( p_wk->p_win, FALSE );
      p_wk->seq = SEQ_END;
    }
    break;
  case SEQ_END:
    p_wk->trg = APPBAR_ICON_RETURN;
    //p_wk->seq = SEQ_TOUCH;
    break;
  }

  APP_TASKMENU_WIN_Update( p_wk->p_win );
}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	�I�����ꂽ���̂��擾
 *
 *	@param	const APPBAR_WORK *cp_wk ���[�N
 *
 *	@return	APPBAR_SELECT��
 */
//-----------------------------------------------------------------------------
APPBAR_ICON APPBAR_GetTrg( const APPBAR_WORK *cp_wk )
{	
	return cp_wk->trg;
}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	�{�^�����A�j�������`�F�b�N
 *
 *	@param	const APPBAR_WORK *cp_wk ���[�N
 *
 *	@return	TRUE�Ȃ�΃A�j����  FALSE�Ȃ�Αҋ@��
 */
//-----------------------------------------------------------------------------
BOOL APPBAR_IsBtnEffect( const APPBAR_WORK *cp_wk )
{	
	return cp_wk->seq > 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �I�����ꂽ���̂𒼂�
 *
 *	@param	APPBAR_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
void APPBAR_SetNormal( APPBAR_WORK *p_wk )
{ 
  APP_TASKMENU_WIN_ResetDecide( p_wk->p_win );
  APP_TASKMENU_WIN_SetActive( p_wk->p_win, FALSE );

  p_wk->seq = 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���J�n���`�F�b�N
 *
 *	@param	const APPBAR_WORK *cp_wk  ���[�N
 *
 *	@return TRUE�A�j���J�n  FALSE�A�j�����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL APPBAR_IsStartAnime( const APPBAR_WORK *cp_wk )
{
  return cp_wk->seq  != 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �{�^���̕\���ݒ�
 *
 *	@param	APPBAR_WORK *p_wk ���[�N
 *	@param	is_visible        TRUE�ŕ\��  FALSE�Ŕ�\��
 */
//-----------------------------------------------------------------------------
void APPBAR_SetVisible( APPBAR_WORK *p_wk, BOOL is_visible )
{ 
  p_wk->is_use  = is_visible;
  GFL_BG_SetVisible( p_wk->bg_frm, is_visible );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �^�b�`����
 *
 *	@param	APPBAR_WORK *p_wk ���[�N
 *	@param	is_use            TRUE�Ŏg�p  FALSE�Ń^�b�`�ł��Ȃ�
 */
//-----------------------------------------------------------------------------
void APPBAR_SetTouchEnable( APPBAR_WORK *p_wk, BOOL is_use )
{
  p_wk->is_use  = is_use;
  p_wk->seq = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �{�^���쐬���Ȃ���
 *
 *	@param	APPBAR_WORK *p_wk ���[�N
 *	@param	type              ���
 */
//-----------------------------------------------------------------------------
void APPBAR_ChangeButton( APPBAR_WORK *p_wk, APPBAR_BUTTON_TYPE type )
{
  if( type != p_wk->btn_type )
  { 
    //�E�B���h�E�j��
    APP_TASKMENU_WIN_Delete( p_wk->p_win );

    //�E�B���h�E�ǂݍ���
    {
      GFL_MSGDATA *p_msg;
      APP_TASKMENU_ITEMWORK item;

      p_msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_irc_compatible_dat, p_wk->heapID );

      GFL_STD_MemClear( &item, sizeof(APP_TASKMENU_ITEMWORK) );
      item.msgColor = APP_TASKMENU_ITEM_MSGCOLOR;

      item.str      = GFL_MSG_CreateString( p_msg, COMPATI_BTN_000 + type );
      item.type     = APP_TASKMENU_WIN_TYPE_RETURN;

      p_wk->p_win       = APP_TASKMENU_WIN_Create( p_wk->p_menu_res, &item, 32-APP_TASKMENU_PLATE_WIDTH, 21, APP_TASKMENU_PLATE_WIDTH, p_wk->heapID );

      GFL_STR_DeleteBuffer( item.str );
      GFL_MSG_Delete( p_msg );

      p_wk->btn_type  = type;
    }

    //��ԃ��Z�b�g
    p_wk->seq = 0;
    p_wk->trg = APPBAR_SELECT_NONE;
  }
}
//=============================================================================
/**
 *			EXTERN
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�ǂݍ���
 *
 *	@param	APPBAR_WORK *p_wk	���[�N
 *	@param	clsys_draw_type		�Z���ǂݍ��݃^�C�v
 *	@param	PALTYPE bgplttype	�p���b�g�ǂݍ��ݏꏊ
 *	@param	mapping						�Z���}�b�s���O���[�h
 *	@param	heapID						�ǂݍ��݃e���|�����p�q�[�vID
 */
//-----------------------------------------------------------------------------
static void AppBar_LoadResource( APPBAR_WORK *p_wk, CLSYS_DRAW_TYPE	clsys_draw_type, PALTYPE bgplttype, APP_COMMON_MAPPING mapping, u8 bg_plt, u8 obj_plt, HEAPID heapID )
{	
#if 0
	ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );

	//BG
	//�̈�̊m��
	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, APP_COMMON_GetBarPltArcIdx(),
			bgplttype, bg_plt*0x20, APPBAR_BG_PLT_NUM*0x20, heapID );
	//�m�ۂ����̈�ɓǂݍ���
	p_wk->chr_pos	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, APP_COMMON_GetBarCharArcIdx(), p_wk->bg_frm, APPBAR_BG_CHARAAREA_SIZE, FALSE, heapID );
	GF_ASSERT_MSG( p_wk->chr_pos != GFL_ARCUTIL_TRANSINFO_FAIL, "APPBAR:BG�L�����̈悪����܂���ł���\n" );
	//�X�N���[���̓�������ɒu���āA����32*3������������
	ARCHDL_UTIL_TransVramScreenEx( p_handle, APP_COMMON_GetBarScrnArcIdx(), p_wk->bg_frm,
			GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos), APPBAR_MENUBAR_X, APPBAR_MENUBAR_Y, 32, 24, 
			APPBAR_MENUBAR_X, APPBAR_MENUBAR_Y, APPBAR_MENUBAR_W, APPBAR_MENUBAR_H,
			bg_plt, FALSE, heapID );


	//OBJ
	//���ʃA�C�R�����\�[�X	
	p_wk->res[APPBAR_RES_COMMON_PLT]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
			APP_COMMON_GetBarIconPltArcIdx(), clsys_draw_type, obj_plt*0x20, 0, APPBAR_OBJ_PLT_NUM, heapID );	
	p_wk->res[APPBAR_RES_COMMON_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
			APP_COMMON_GetBarIconCharArcIdx(), FALSE, clsys_draw_type, heapID );

	p_wk->res[APPBAR_RES_COMMON_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
			APP_COMMON_GetBarIconCellArcIdx(mapping),
			APP_COMMON_GetBarIconAnimeArcIdx(mapping), heapID );

	GFL_ARC_CloseDataHandle( p_handle );
#endif

	ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCCOMPATIBLE, heapID );

  p_wk->res[APPBAR_RES_BAR_PLT]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
			NARC_irccompatible_gra_shita_bar_NCLR, clsys_draw_type, obj_plt*0x20, 0, 1, heapID );	
	p_wk->res[APPBAR_RES_BAR_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
			NARC_irccompatible_gra_shita_bar_NCGR, FALSE, clsys_draw_type, heapID );

	p_wk->res[APPBAR_RES_BAR_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
			NARC_irccompatible_gra_shita_bar_NCER,
			NARC_irccompatible_gra_shita_bar_NANR, heapID );

	GFL_ARC_CloseDataHandle( p_handle );

}

//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�j��
 *	
 *	@param	APPBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void AppBar_UnLoadResource( APPBAR_WORK *p_wk )
{	
#if 0
	//OBJ
	GFL_CLGRP_CELLANIM_Release( p_wk->res[APPBAR_RES_COMMON_CEL] );
	GFL_CLGRP_CGR_Release( p_wk->res[APPBAR_RES_COMMON_CHR] );
	GFL_CLGRP_PLTT_Release( p_wk->res[APPBAR_RES_COMMON_PLT] );

	//BG
	GFL_BG_FreeCharacterArea(p_wk->bg_frm,
			GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos),
			GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->chr_pos));
#endif
	GFL_CLGRP_CELLANIM_Release( p_wk->res[APPBAR_RES_BAR_CEL] );
	GFL_CLGRP_CGR_Release( p_wk->res[APPBAR_RES_BAR_CHR] );
	GFL_CLGRP_PLTT_Release( p_wk->res[APPBAR_RES_BAR_PLT] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�쐬
 *
 *	@param	APPBAR_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void AppBar_CreateObj( APPBAR_WORK *p_wk, GFL_CLUNIT* p_unit, APPBAR_OPTION_MASK mask, CLSYS_DEFREND_TYPE	clsys_def_type, u8 bg_pri, HEAPID heapID )
{	
	//CLWK�̍쐬
	int i;
	GFL_CLWK_DATA				cldata;

	GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

  cldata.pos_x	= 128;
  cldata.pos_y	= 96;
  cldata.anmseq	= 0;
  cldata.bgpri  = bg_pri+1;
  p_wk->p_clwk[0]	= GFL_CLACT_WK_Create( p_unit, 
      p_wk->res[APPBAR_RES_BAR_CHR],
      p_wk->res[APPBAR_RES_BAR_PLT],
      p_wk->res[APPBAR_RES_BAR_CEL],
      &cldata,
      clsys_def_type,
      heapID
      );
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�j��
 *
 *	@param	APPBAR_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void AppBar_DeleteObj( APPBAR_WORK *p_wk )
{	
	int i;
	for( i = 0; i < APPBAR_ICON_MAX; i++ )
	{	
		if( p_wk->p_clwk[i] )
		{	
			GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	Screen�f�[�^�́@VRAM�]���g���Łi�ǂݍ��񂾃X�N���[���̈ꕔ�͈͂��o�b�t�@�ɒ������j
 *
 *	@param	ARCHANDLE *handle	�n���h��
 *	@param	datID							�f�[�^ID
 *	@param	frm								�t���[��
 *	@param	chr_ofs						�L�����I�t�Z�b�g
 *	@param	src_x							�]���������W
 *	@param	src_y							�]����Y���W
 *	@param	src_w							�]������			//�f�[�^�̑S�̂̑傫��
 *	@param	src_h							�]��������		//�f�[�^�̑S�̂̑傫��
 *	@param	dst_x							�]����X���W
 *	@param	dst_y							�]����Y���W
 *	@param	dst_w							�]���敝
 *	@param	dst_h							�]���捂��
 *	@param	plt								�p���b�g�ԍ�
 *	@param	compressedFlag		���k�t���O
 *	@param	heapID						�ꎞ�o�b�t�@�p�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h, u8 plt, BOOL compressedFlag, HEAPID heapID )
{	
	void *p_src_arc;
	NNSG2dScreenData* p_scr_data;

	//�ǂݍ���
	p_src_arc = GFL_ARCHDL_UTIL_Load( handle, datID, compressedFlag, GetHeapLowID(heapID) );

	//�A���p�b�N
	if(!NNS_G2dGetUnpackedScreenData( p_src_arc, &p_scr_data ) )
	{	
		GF_ASSERT(0);	//�X�N���[���f�[�^����Ȃ���
	}

	//�G���[
	GF_ASSERT( src_w * src_h * 2 <= p_scr_data->szByte );

	//�L�����I�t�Z�b�g�v�Z
	if( chr_ofs )
	{	
		int i;
		if( GFL_BG_GetScreenColorMode( frm ) == GX_BG_COLORMODE_16 )
		{
			u16 *p_scr16;
			
			p_scr16	=	(u16 *)&p_scr_data->rawData;
			for( i = 0; i < src_w * src_h ; i++ )
			{
				p_scr16[i]	+= chr_ofs;
			}	
		}
		else
		{	
			GF_ASSERT(0);	//256�ł͍���ĂȂ�
		}
	}

	//��������
	if( GFL_BG_GetScreenBufferAdrs( frm ) != NULL )
	{	
		GFL_BG_WriteScreenExpand( frm, dst_x, dst_y, dst_w, dst_h,
				&p_scr_data->rawData, src_x, src_y, src_w, src_h );	
		GFL_BG_ChangeScreenPalette( frm, dst_x, dst_y, dst_w, dst_h, plt );
		GFL_BG_LoadScreenReq( frm );
	}
	else
	{	
		GF_ASSERT(0);	//�o�b�t�@���Ȃ�
	}

	GFL_HEAP_FreeMemory( p_src_arc );
}	

