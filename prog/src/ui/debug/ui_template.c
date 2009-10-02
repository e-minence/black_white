//=============================================================================
/**
 *
 *	@file		ui_template.c
 *	@brief  �V�K�A�v���p�e���v���[�g
 *	@author	Toru=Nagihashi / genya hosaka
 *	@data		2009.09.29
 *
 */
//=============================================================================
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

//FONT
#include "print/gf_font.h"
#include "font/font.naix"

//PRINT_QUE
#include "print/printsys.h"

#include "print/wordset.h"

//INFOWIN
#include "infowin/infowin.h"

//�`��ݒ�
#include "ui_template_graphic.h"

//�e�N�X�`����OAM�ɓ]��
#include "ui/nsbtx_to_clwk.h"

//�}�b�v���f����nsbtx
#include "fieldmap/fldmmdl_mdlres.naix"

//�^�b�`�o�[
#include "ui/touchbar.h"

//�|�P�p���A�|�P����NO
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

//MCSS
#include "system/mcss.h"
#include "system/mcss_tool.h"

//�^�X�N���j���[
#include "app/app_taskmenu.h"

//�ǂ����A�C�R��
#include "item/item.h"
#include "item_icon.naix"

//�A�v�����ʑf��
#include "app/app_menu_common.h"

//�A�[�J�C�u
#include "arc_def.h"

//�O�����J
#include "ui_template.h"


//@TODO BG�ǂݍ��� �Ƃ肠�����}�C�N�e�X�g�̃��\�[�X
#include "message.naix"
#include "mictest.naix"	// �A�[�J�C�u
#include "msg/msg_mictest.h"  // GMM

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================
#define UI_TEMPLATE_POKE_ICON
#define UI_TEMPLATE_INFOWIN
#define UI_TEMPLATE_TOUCHBAR
#define UI_TEMPLATE_MCSS
#define UI_TEMPLATE_TASKMENU
#define UI_TEMPLATE_TYPEICON      // ���ށA�Z�A�C�R��
#define UI_TEMPLATE_ITEM_ICON     // �ǂ����A�C�R��
#define UI_TEMPLATE_OAM_MAPMODEL  // �}�b�v���f����OAM�ŕ\��

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  UI_TEMPLATE_HEAP_SIZE = 0x20000,  ///< �q�[�v�T�C�Y
};

//-------------------------------------
///	�t���[��
//=====================================
enum
{	
	BG_FRAME_BAR_M	= GFL_BG_FRAME1_M,
	BG_FRAME_BACK_M	= GFL_BG_FRAME2_M,
	BG_FRAME_BACK_S	= GFL_BG_FRAME2_S,
};
//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	//���C��BG
	PLTID_BG_BACK_M				= 0,
	PLTID_BG_TASKMENU_M		= 11,
	PLTID_BG_TOUCHBAR_M		= 13,
	PLTID_BG_INFOWIN_M		= 15,
	//�T�uBG
	PLTID_BG_BACK_S				=	0,

	//���C��OBJ
	PLTID_OBJ_TOUCHBAR_M	= 0, // 3�{�g�p
	PLTID_OBJ_TYPEICON_M	= 3, // 3�{�g�p
  PLTID_OBJ_OAM_MAPMODEL_M = 6, // 1�{�g�p
  PLTID_OBJ_ITEMICON_M = 7,     // 1�{�g�p
	//�T�uOBJ
};

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
//--------------------------------------------------------------
///	BG�Ǘ����[�N
//==============================================================
typedef struct 
{
	int dummy;
} UI_TEMPLATE_BG_WORK;

//--------------------------------------------------------------
///	CLWK�Ǘ����[�N
//==============================================================
typedef struct {
  u32       res_ncg;
  u32       res_ncl;
  u32       res_nce;
  GFL_CLWK* clwk;
} UI_TEMPLATE_CLWK_SET;

//--------------------------------------------------------------
///	CLWK�������p�����[�^
//==============================================================
typedef struct {
  CLSYS_DRAW_TYPE draw_type; ///< �`���^�C�v
  u32 arc_id;   ///< �A�[�J�C�uNO
  u32 pltt_id;  ///< �p���b�g���\�[�XNO
  u32 ncg_id;   ///< �L�����N�^���\�[�XNO
  u32 cell_id;  ///< �Z�����\�[�XNO
  u32 anm_id;   ///< �Z���A�j�����\�[�XNO
  u8 pltt_line; ///< �p���b�g�]����NO
  u8 px;        ///< X���W
  u8 py;        ///< Y���W
  u8 padding[1];
} CLWK_SET_PARAM;


//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heap_id;

	UI_TEMPLATE_BG_WORK				wk_bg;

	//�`��ݒ�
	UI_TEMPLATE_GRAPHIC_WORK	*graphic;

#ifdef UI_TEMPLATE_TOUCHBAR
	//�^�b�`�o�[
	TOUCHBAR_WORK							*touchbar;
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_MCSS
	//MCSS
	MCSS_SYS_WORK							*mcss_sys;
	MCSS_WORK									*mcss_wk;
#endif //UI_TEMPLATE_MSCC

	//�t�H���g
	GFL_FONT									*font;

	//�v�����g�L���[
	PRINT_QUE									*print_que;
	GFL_MSGDATA								*msg;

#ifdef UI_TEMPLATE_TASKMENU
	//�^�X�N���j���[
	APP_TASKMENU_RES					*menu_res;
	APP_TASKMENU_WORK					*menu;
#endif //UI_TEMPLATE_TASKMENU

#ifdef UI_TEMPLATE_TYPEICON
	//���ށA�Z�A�C�R��
  UI_TEMPLATE_CLWK_SET      type_clwk_set;
	u32												type_ncg;
	u32												type_ncl;
	u32												type_nce;
	GFL_CLWK									*type_clwk;
#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
  UI_TEMPLATE_CLWK_SET      clset_oam_mmdl;
#endif // UI_TEMPLATE_OAM_MAPMODEL

#ifdef UI_TEMPLATE_ITEM_ICON
  UI_TEMPLATE_CLWK_SET      clset_item_icon;
#endif //UI_TEMPLATE_ITEM_ICON


#ifdef	UI_TEMPLATE_PRINT_TOOL
	//�v�����g���[�e�B���e�B
	PRINT_UTIL								print_util;
	u32												seq;
#endif	// UI_TEMPLATE_PRINT_TOOL

} UI_TEMPLATE_MAIN_WORK;


//=============================================================================
/**
 *							�f�[�^��`
 */
//=============================================================================


//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
static GFL_PROC_RESULT UITemplateProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT UITemplateProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT UITemplateProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
//-------------------------------------
///	�ėp�������[�e�B���e�B
//=====================================
static void UITemplate_BG_LoadResource( UI_TEMPLATE_BG_WORK* wk, HEAPID heap_id );
static void UITemplate_OBJ_CreateCLWK( UI_TEMPLATE_CLWK_SET* wk, CLWK_SET_PARAM* prm, GFL_CLUNIT* unit, HEAPID heap_id );
static void UItemplate_OBJ_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk );
//-------------------------------------
///	INFOWIN
//=====================================
static void UITemplate_INFOWIN_Init( GAMESYS_WORK *gamesys, HEAPID heap_id );
static void UITemplate_INFOWIN_Exit( void );
static void UITemplate_INFOWIN_Main( void );
//-------------------------------------
///	�^�b�`�o�[
//=====================================
static TOUCHBAR_WORK * UITemplate_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heap_id );
static void UITemplate_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar );
static void UITemplate_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar );
//-------------------------------------
///	MCSS
//=====================================
static MCSS_SYS_WORK * UITemplate_MCSS_Init( u16 wk_max, HEAPID heap_id );
static void UITemplate_MCSS_Exit( MCSS_SYS_WORK * mcss );
static void UITemplate_MCSS_Draw( MCSS_SYS_WORK * mcss );
static MCSS_WORK * UITemplate_MCSS_CreateWkPP( MCSS_SYS_WORK * mcss, POKEMON_PARAM *pp, const VecFx32 *pos );
static void UITemplate_MCSS_DeleteWk( MCSS_SYS_WORK * mcss, MCSS_WORK *wk );
//-------------------------------------
///	���X�g�V�X�e���͂��A������
//=====================================
static APP_TASKMENU_WORK * UITemplate_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *p_msg, HEAPID heap_id );
static void UITemplate_TASKMENU_Exit( APP_TASKMENU_WORK *menu );
static void UITemplate_TASKMENU_Main( APP_TASKMENU_WORK *menu );
//-------------------------------------
///	���ށA�Z�A�C�R��
//=====================================
static void UITemplate_TYPEICON_CreateCLWK( UI_TEMPLATE_MAIN_WORK *wk, PokeType type, GFL_CLUNIT *unit, HEAPID heap_id );
static void UITemplate_TYPEICON_DeleteCLWK( UI_TEMPLATE_MAIN_WORK *wk );

#ifdef UI_TEMPLATE_ITEM_ICON
//-------------------------------------
///	�ǂ����A�C�R��
//=====================================
static void UITemplate_ITEM_ICON_CreateCLWK( UI_TEMPLATE_CLWK_SET* wk, u16 item_id, GFL_CLUNIT* unit, HEAPID heap_id );
static void UITemplate_ITEM_ICON_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk );
#endif // UI_TEMPLATE_ITEM_ICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
//-------------------------------------
///	OAM�Ń}�b�v���f���\��
//=====================================
static void UITemplate_OAM_MAPMODEL_CreateCLWK( UI_TEMPLATE_CLWK_SET *wk, u16 tex_idx, u8 ptn_ofs, GFL_CLUNIT *unit, HEAPID heap_id );
static void UITemplate_OAM_MAPMODEL_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk );
#endif // UI_TEMPLATE_OAM_MAPMODEL

//=============================================================================
/**
 *								�O�����J
 */
//=============================================================================
const GFL_PROC_DATA UITemplateProcData = 
{
	UITemplateProc_Init,
	UITemplateProc_Main,
	UITemplateProc_Exit,
};
//=============================================================================
/**
 *								PROC
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC ����������
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					UI_TEMPLATE_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UITemplateProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
	UI_TEMPLATE_MAIN_WORK *wk;
	UI_TEMPLATE_PARAM *param;

	//�I�[�o�[���C�ǂݍ���
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	
	//�����擾
	param	= pwk;

	//�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_UI_DEBUG, UI_TEMPLATE_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(UI_TEMPLATE_MAIN_WORK), HEAPID_UI_DEBUG );
  GFL_STD_MemClear( wk, sizeof(UI_TEMPLATE_MAIN_WORK) );

  // ������
  wk->heap_id = HEAPID_UI_DEBUG;
	
	//�`��ݒ菉����
	wk->graphic	= UI_TEMPLATE_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heap_id );

	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heap_id );

	//���b�Z�[�W
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
			NARC_message_mictest_dat, wk->heap_id );

	//PRINT_QUE�쐬
	wk->print_que		= PRINTSYS_QUE_Create( wk->heap_id );

	//BG���\�[�X�ǂݍ���
	UITemplate_BG_LoadResource( &wk->wk_bg, wk->heap_id );

#ifdef UI_TEMPLATE_INFOWIN
	//INFOWIN�̏�����
	UITemplate_INFOWIN_Init( param->gamesys, wk->heap_id );
#endif //UI_TEMPLATE_INFOWIN

#ifdef UI_TEMPLATE_TOUCHBAR
	//�^�b�`�o�[�̏�����
	{	
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
		wk->touchbar	= UITemplate_TOUCHBAR_Init( clunit, wk->heap_id );
	}
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_MCSS
	//MCSS������&���[�N�쐬
	wk->mcss_sys	= UITemplate_MCSS_Init( 1, wk->heap_id );
	{
		POKEMON_PARAM	*pp;  
		VecFx32 pos	= { 0, 0, 0	};

		pp	= PP_Create( MONSNO_HUSIGIDANE, 0, 0, wk->heap_id );
		wk->mcss_wk		= UITemplate_MCSS_CreateWkPP( wk->mcss_sys, pp, &pos );
		GFL_HEAP_FreeMemory( pp );
	}
#endif //UI_TEMPLATE_MSCC

#ifdef UI_TEMPLATE_TASKMENU
	//TASKMENU���\�[�X�ǂݍ��݁�������
	wk->menu_res	= APP_TASKMENU_RES_Create( BG_FRAME_BAR_M, PLTID_BG_TASKMENU_M, wk->font, wk->print_que, wk->heap_id );
	wk->menu			= UITemplate_TASKMENU_Init( wk->menu_res, wk->msg, wk->heap_id );
#endif //UI_TEMPLATE_TASKMENU

#ifdef UI_TEMPLATE_TYPEICON
	//�����A�C�R���̓ǂݍ���
	{	
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
		UITemplate_TYPEICON_CreateCLWK( wk, POKETYPE_KUSA, clunit, wk->heap_id );
	}
#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
  //OAM�}�b�v���f���̓ǂݍ���
  {
    // ��l���͌��� 0=���, 1=���A�j��1, 2=���A�j��2, 3=����....
    // �g���[�i�[�Ȃǂ͈Ⴄ�\��������̂Œ��ӁB
    u16 ptn_ofs = 3;
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );

		UITemplate_OAM_MAPMODEL_CreateCLWK( &wk->clset_oam_mmdl, NARC_fldmmdl_mdlres_hero_nsbtx, ptn_ofs, clunit, wk->heap_id );
  }
#endif //UI_TEMPLATE_OAM_MAPMODEL

#ifdef UI_TEMPLATE_ITEM_ICON
  // �ǂ����A�C�R���̓ǂݍ���
  { 
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
    UITemplate_ITEM_ICON_CreateCLWK( &wk->clset_item_icon, ITEM_KIZUGUSURI, clunit, wk->heap_id );
  }
#endif //UI_TEMPLATE_ITEM_ICON

	//@todo	�t�F�[�h�V�[�P���X���Ȃ��̂�
	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(0);

  return GFL_PROC_RES_FINISH;
}
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC �I������
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					UI_TEMPLATE_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UITemplateProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	UI_TEMPLATE_MAIN_WORK* wk = mywk;

#ifdef UI_TEMPLATE_TYPEICON
	//�����A�C�R���̔j��
	UITemplate_TYPEICON_DeleteCLWK( wk );
#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_ITEM_ICON
#endif //UI_TEMPLATE_ITEM_ICON

#ifdef UI_TEMPLATE_ITEM_ICON
  //�ǂ����A�C�R���̔j��
  UITemplate_ITEM_ICON_DeleteCLWK( &wk->clset_item_icon );
#endif //UI_TEMPLATE_ITEM_ICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
  //OAM�}�b�v���f���̔j��
  UITemplate_OAM_MAPMODEL_DeleteCLWK( &wk->clset_oam_mmdl );
#endif //UI_TEMPLATE_OAM_MAPMODEL

#ifdef UI_TEMPLATE_TASKMENU
	//TASKMENU�V�X�e�������\�[�X�j��
	UITemplate_TASKMENU_Exit( wk->menu );
	APP_TASKMENU_RES_Delete( wk->menu_res );	
#endif //UI_TEMPLATE_TASKMENU

#ifdef UI_TEMPLATE_MCSS
	//MSCC�j��
	UITemplate_MCSS_DeleteWk( wk->mcss_sys, wk->mcss_wk );
	UITemplate_MCSS_Exit( wk->mcss_sys );
#endif //UI_TEMPLATE_MCSS

#ifdef UI_TEMPLATE_TOUCHBAR
	//�^�b�`�o�[
	UITemplate_TOUCHBAR_Exit( wk->touchbar );
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_INFOWIN
	//INFWIN
	UITemplate_INFOWIN_Exit();
#endif //UI_TEMPLATE_INFOWIN

	//���b�Z�[�W�j��
	GFL_MSG_Delete( wk->msg );

	//PRINT_QUE
	PRINTSYS_QUE_Delete( wk->print_que );

	//FONT
	GFL_FONT_Delete( wk->font );

	//�`��ݒ�j��
	UI_TEMPLATE_GRAPHIC_Exit( wk->graphic );

	//PROC�p���������
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( wk->heap_id );

	//�I�[�o�[���C�j��
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

  return GFL_PROC_RES_FINISH;
}
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC �又��
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					UI_TEMPLATE_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UITemplateProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	UI_TEMPLATE_MAIN_WORK* wk = mywk;

  // �f�o�b�O�{�^���ŃA�v���I��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG )
  {
    return GFL_PROC_RES_FINISH;
  }

#ifdef UI_TEMPLATE_TOUCHBAR
	//�^�b�`�o�[���C������
	UITemplate_TOUCHBAR_Main( wk->touchbar );
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_INFOWIN
	//INFWIN���C������
	UITemplate_INFOWIN_Main();
#endif //UI_TEMPLATE_INFOWIN

#ifdef UI_TEMPLATE_TASKMENU
	//�^�X�N���j���[���C������
	UITemplate_TASKMENU_Main( wk->menu );
#endif //UI_TEMPLATE_TASKMENU

	//PRINT_QUE
	PRINTSYS_QUE_Main( wk->print_que );

	//2D�`��
	UI_TEMPLATE_GRAPHIC_2D_Draw( wk->graphic );

	//3D�`��
	UI_TEMPLATE_GRAPHIC_3D_StartDraw( wk->graphic );
#ifdef UI_TEMPLATE_MCSS
	UITemplate_MCSS_Draw( wk->mcss_sys );
#endif //UI_TEMPLATE_MCSS
	UI_TEMPLATE_GRAPHIC_3D_EndDraw( wk->graphic );


  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *	@brief  BG�Ǘ����W���[�� ���\�[�X�ǂݍ���
 *
 *	@param	UI_TEMPLATE_BG_WORK* wk BG�Ǘ����[�N
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_BG_LoadResource( UI_TEMPLATE_BG_WORK* wk, HEAPID heap_id )
{
  //@TODO �Ƃ肠�����}�C�N�e�X�g�̃��\�[�X
	ARCHANDLE	*handle;

	handle	= GFL_ARC_OpenDataHandle( ARCID_MICTEST_GRA, heap_id );

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_mictest_back_bg_down_NCLR, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, 0x20, heap_id );
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_mictest_back_bg_up_NCLR, PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0x20, heap_id );
	
  //	----- ����� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_mictest_back_bg_down_NCGR,
						BG_FRAME_BACK_S, 0, 0, 0, heap_id );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_mictest_back_bg_down_NSCR,
						BG_FRAME_BACK_S, 0, 0, 0, heap_id );	

	//	----- ���� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_mictest_back_bg_up_NCGR,
						BG_FRAME_BACK_M, 0, 0, 0, heap_id );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_mictest_back_bg_up_NSCR,
						BG_FRAME_BACK_M, 0, 0, 0, heap_id );		

	GFL_ARC_CloseDataHandle( handle );
}


//-----------------------------------------------------------------------------
/**
 *	@brief  �ėp���\�[�X�ǂݍ��݁��A�N�^�[��������
 *
 *	@param	UI_TEMPLATE_CLWK_SET* wk
 *	@param	prm
 *	@param	unit
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_OBJ_CreateCLWK( UI_TEMPLATE_CLWK_SET* wk, CLWK_SET_PARAM* prm, GFL_CLUNIT* unit, HEAPID heap_id )
{
  //���\�[�X�ǂݍ���
	{	
		ARCHANDLE *p_handle;
  	
		p_handle	= GFL_ARC_OpenDataHandle( prm->arc_id, heap_id );

		wk->res_ncl	= GFL_CLGRP_PLTT_Register( p_handle,
        prm->pltt_id, prm->draw_type, prm->pltt_line*0x20, heap_id );

		wk->res_ncg	= GFL_CLGRP_CGR_Register( p_handle,
        prm->ncg_id, FALSE, prm->draw_type, heap_id );

		wk->res_nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
        prm->cell_id,	prm->anm_id, heap_id );

		GFL_ARC_CloseDataHandle( p_handle );	
	}

	//CLWK�쐬
	{	
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.pos_x	= prm->px;
		cldata.pos_y	= prm->py;
		wk->clwk = GFL_CLACT_WK_Create( unit,
				wk->res_ncg,
				wk->res_ncl,
				wk->res_nce,
				&cldata,
				prm->draw_type,
				heap_id );
	}
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ėp���\�[�X�ǂݍ��݁��A�N�^�[�폜����
 *
 *	@param	UI_TEMPLATE_CLWK_SET* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UItemplate_OBJ_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk )
{
	//CLWK�j��
	GFL_CLACT_WK_Remove( wk->clwk );

	//���\�[�X�j��
	GFL_CLGRP_PLTT_Release( wk->res_ncl );
	GFL_CLGRP_CGR_Release( wk->res_ncg );
	GFL_CLGRP_CELLANIM_Release( wk->res_nce );
}




//=============================================================================
/**
 *		INFOWIN
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	INFOWIN������
 *
 *	@param	GAMESYS_WORK *gamesys	�Q�[���V�X�e��
 *	@param	heap_id		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void UITemplate_INFOWIN_Init( GAMESYS_WORK *gamesys, HEAPID heap_id )
{	
	GAME_COMM_SYS_PTR comm;
	comm	= GAMESYSTEM_GetGameCommSysPtr(gamesys);
	INFOWIN_Init( BG_FRAME_BAR_M, PLTID_BG_INFOWIN_M, comm, heap_id );
}
//----------------------------------------------------------------------------
/**
 *	@brief	INFOWIN�j��
 *
 *	@param	void 
 */
//-----------------------------------------------------------------------------
static void UITemplate_INFOWIN_Exit( void )
{	
	INFOWIN_Exit();
}
//----------------------------------------------------------------------------
/**
 *	@brief	INFWIN���C������
 *
 *	@param	void 
 */
//-----------------------------------------------------------------------------
static void UITemplate_INFOWIN_Main( void )
{	
	INFOWIN_Update();
}
//=============================================================================
/**
 *	TOUCHBAR
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR������
 *
 *	@param	GFL_CLUNIT *clunit	CLUNIT
 *	@param	heap_id							�q�[�vID
 *
 *	@return	TOUCHBAR_WORK
 */
//-----------------------------------------------------------------------------
static TOUCHBAR_WORK * UITemplate_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heap_id )
{	
	//�A�C�R���̐ݒ�
	//�������
	TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]	=
	{	
		{	
			TOUCHBAR_ICON_RETURN,
			{	TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
		},
		{	
			TOUCHBAR_ICON_CLOSE,
			{	TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_Y },
		},
		{	
			TOUCHBAR_ICON_CHECK,
			{	TOUCHBAR_ICON_X_05, TOUCHBAR_ICON_Y_CHECK },
		},
	};

	//�ݒ�\����
	//�����قǂ̑����{���\�[�X���������
	TOUCHBAR_SETUP	touchbar_setup;
	touchbar_setup.p_item		= touchbar_icon_tbl;				//��̑����
	touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);//�����������邩
	touchbar_setup.p_unit		= clunit;										//OBJ�ǂݍ��݂̂��߂�CLUNIT
	touchbar_setup.bar_frm	= BG_FRAME_BAR_M;						//BG�ǂݍ��݂̂��߂�BG��
	touchbar_setup.bg_plt		= PLTID_BG_TOUCHBAR_M;			//BG��گ�
	touchbar_setup.obj_plt	= PLTID_OBJ_TOUCHBAR_M;			//OBJ��گ�
	touchbar_setup.mapping	= APP_COMMON_MAPPING_128K;	//�}�b�s���O���[�h

	return TOUCHBAR_Init( &touchbar_setup, heap_id );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR�j��
 *
 *	@param	TOUCHBAR_WORK	*touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void UITemplate_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Exit( touchbar );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR���C������
 *
 *	@param	TOUCHBAR_WORK	*touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void UITemplate_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Main( touchbar );
}
//=============================================================================
/**
 *		MCSS
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	MCSS������
 *
 *	@param	u16 wk_max		MSCC�̃��[�N�쐬�ő吔
 *	@param	heap_id				�q�[�vID
 *
 *	@return	MCSS_SYS
 */
//-----------------------------------------------------------------------------
static MCSS_SYS_WORK * UITemplate_MCSS_Init( u16 wk_max, HEAPID heap_id )
{	
	MCSS_SYS_WORK *mcss;
	mcss = MCSS_Init( wk_max , heap_id );
	MCSS_SetTextureTransAdrs( mcss, 0 );
	MCSS_SetOrthoMode( mcss );
	return mcss;
}
//----------------------------------------------------------------------------
/**
 *	@brief	MCSS�j��
 *
 *	@param	MCSS_SYS_WORK * mcss	MCSS�QSYS
 */
//-----------------------------------------------------------------------------
static void UITemplate_MCSS_Exit( MCSS_SYS_WORK * mcss )
{	
	MCSS_Exit(mcss);
}
//----------------------------------------------------------------------------
/**
 *	@brief	MCSS�`��
 *
 *	@param	MCSS_SYS_WORK * mcss MCSS�QSYS
 */
//-----------------------------------------------------------------------------
static void UITemplate_MCSS_Draw( MCSS_SYS_WORK * mcss )
{	
	MCSS_Main( mcss );
	MCSS_Draw( mcss );
}
//----------------------------------------------------------------------------
/**
 *	@brief	MCSS���[�N�쐬
 *
 *	@param	MCSS_SYS_WORK * mcss	MCSS_SYS
 *	@param	*ppp					�|�P�����p�[�\�i���p�����[�^
 *	@param	VecFx32 *pos	�ʒu
 *
 *	@return	MCSS_WK
 */
//-----------------------------------------------------------------------------
static MCSS_WORK * UITemplate_MCSS_CreateWkPP( MCSS_SYS_WORK * mcss, POKEMON_PARAM *pp, const VecFx32 *pos )
{	
	MCSS_WORK *poke;
	MCSS_ADD_WORK addWork;

	VecFx32 scale = {FX32_ONE*16,FX32_ONE*16,FX32_ONE};

	MCSS_TOOL_MakeMAWPP( pp , &addWork , MCSS_DIR_FRONT );
	poke = MCSS_Add( mcss, pos->x, pos->y , pos->z, &addWork );
	MCSS_SetScale( poke , &scale );
	return poke;
}
//----------------------------------------------------------------------------
/**
 *	@brief	MCSS���[�N�j��
 *
 *	@param	MCSS_SYS_WORK * mcss	MCSS_SYS
 *	@param	*wk										���[�N
 *
 */
//-----------------------------------------------------------------------------
static void UITemplate_MCSS_DeleteWk( MCSS_SYS_WORK * mcss, MCSS_WORK *wk )
{	
	MCSS_SetVanishFlag( wk );
	MCSS_Del(mcss,wk);
}
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENU�̏�����
 *
 *	@param	menu_res	���\�[�X
 */
//-----------------------------------------------------------------------------
static APP_TASKMENU_WORK * UITemplate_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *p_msg, HEAPID heap_id )
{	
	int i;
	APP_TASKMENU_INITWORK	init;
	APP_TASKMENU_ITEMWORK	item[3];
	APP_TASKMENU_WORK			*menu;	

	//���̐ݒ�
	for( i = 0; i < NELEMS(item); i++ )
	{	
		item[i].str	= GFL_MSG_CreateString( p_msg, 0 );	//������
		item[i].msgColor	= APP_TASKMENU_ITEM_MSGCOLOR;	//�����F
		item[i].type			= APP_TASKMENU_WIN_TYPE_NORMAL;	//���̎��
	}

	//������
	init.heapId		= heap_id;
	init.itemNum	= NELEMS(item);
	init.itemWork = item;
	init.posType	= ATPT_RIGHT_DOWN;
	init.charPosX	= 32;
	init.charPosY = 21;

	menu	= APP_TASKMENU_OpenMenu( &init, menu_res );

	//��������
	for( i = 0; i < NELEMS(item); i++ )
	{	
		GFL_STR_DeleteBuffer( item[i].str );
	}

	return menu;

}
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENU�̔j��
 *
 *	@param	APP_TASKMENU_WORK *menu	���[�N
 */
//-----------------------------------------------------------------------------
static void UITemplate_TASKMENU_Exit( APP_TASKMENU_WORK *menu )
{	
	APP_TASKMENU_CloseMenu( menu );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENU�̃��C������
 *
 *	@param	APP_TASKMENU_WORK *menu	���[�N
 */
//-----------------------------------------------------------------------------
static void UITemplate_TASKMENU_Main( APP_TASKMENU_WORK *menu )
{	
	APP_TASKMENU_UpdateMenu( menu );
}

#ifdef UI_TEMPLATE_TYPEICON
//=============================================================================
/**
 *	TYPEICON
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�����A�^�C�v�A�C�R���쐬
 *
 *	@param	PokeType					�^�C�v
 *	@param	GFL_CLUNIT *unit	CLUNIT
 *	@param	heap_id						�q�[�vID
 *
 *	@return	CLWK
 */
//-----------------------------------------------------------------------------
static void UITemplate_TYPEICON_CreateCLWK( UI_TEMPLATE_MAIN_WORK *wk, PokeType type, GFL_CLUNIT *unit, HEAPID heap_id )
{	
	//���\�[�X�ǂݍ���
	{	
		ARCHANDLE *p_handle;

		p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heap_id );

		wk->type_ncl	= GFL_CLGRP_PLTT_Register( p_handle, 
				APP_COMMON_GetPokeTypePltArcIdx(), CLSYS_DRAW_MAIN, PLTID_OBJ_TYPEICON_M*0x20, heap_id );

		wk->type_ncg	= GFL_CLGRP_CGR_Register( p_handle,
				APP_COMMON_GetPokeTypeCharArcIdx(type), FALSE, CLSYS_DRAW_MAIN, heap_id );

		wk->type_nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
				APP_COMMON_GetPokeTypeCellArcIdx(APP_COMMON_MAPPING_128K ),
				APP_COMMON_GetPokeTypeAnimeArcIdx( APP_COMMON_MAPPING_128K ), heap_id );


		GFL_ARC_CloseDataHandle( p_handle );	
	}


	//CLWK�쐬
	{	
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.pos_x	= 128;
		cldata.pos_y	= 50;
		wk->type_clwk	=		GFL_CLACT_WK_Create( unit,
				wk->type_ncg,
				wk->type_ncl,
				wk->type_nce,
				&cldata,
				CLSYS_DEFREND_MAIN,
				heap_id );
		GFL_CLACT_WK_SetPlttOffs( wk->type_clwk, APP_COMMON_GetPokeTypePltOffset(type),
				CLWK_PLTTOFFS_MODE_PLTT_TOP );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����A�^�C�v�A�C�R���j��
 *
 *	@param	GFL_CLUNIT *unit	CLUNIT
 *	@param	GFL_CLWK	 *clwk	CLWK
 */
//-----------------------------------------------------------------------------
static void UITemplate_TYPEICON_DeleteCLWK( UI_TEMPLATE_MAIN_WORK *wk )
{	
	//CLWK�j��
	GFL_CLACT_WK_Remove( wk->type_clwk );

	//���\�[�X�j��
	GFL_CLGRP_PLTT_Release( wk->type_ncl );
	GFL_CLGRP_CGR_Release( wk->type_ncg );
	GFL_CLGRP_CELLANIM_Release( wk->type_nce );
}

#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_ITEM_ICON

//-----------------------------------------------------------------------------
/**
 *	@brief  �ǂ����A�C�R������
 *
 *	@param	UI_TEMPLATE_CLWK_SET* wk
 *	@param	item_id
 *	@param	unit
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_ITEM_ICON_CreateCLWK( UI_TEMPLATE_CLWK_SET* wk, u16 item_id, GFL_CLUNIT* unit, HEAPID heap_id )
{	
  CLWK_SET_PARAM prm;

  prm.arc_id    = ARCID_ITEMICON;
  prm.pltt_id   = ITEM_GetIndex( item_id, ITEM_GET_ICON_PAL );
  prm.ncg_id    = ITEM_GetIndex( item_id, ITEM_GET_ICON_CGX );
  prm.cell_id   = NARC_item_icon_itemicon_NCER;
  prm.anm_id    = NARC_item_icon_itemicon_NANR;
  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.pltt_line = PLTID_OBJ_ITEMICON_M;
  prm.px        = 32 + 16;
  prm.py        = 32;

  UITemplate_OBJ_CreateCLWK( wk, &prm, unit, heap_id );
}

//-----------------------------------------------------------------------------
/**
 *	@brief	�ǂ����A�C�R���j��
 *
 *	@param	UI_TEMPLATE_CLWK_SET* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_ITEM_ICON_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk )
{
  UItemplate_OBJ_DeleteCLWK( wk );
}

#endif // UI_TEMPLATE_ITEM_ICON


#ifdef UI_TEMPLATE_OAM_MAPMODEL
//=============================================================================
/**
 *	OAM_MAPMODEL
 */
//=============================================================================


//-----------------------------------------------------------------------------
/**
 *	@brief  OAM�}�b�v���f�� �쐬 (�l��OBJ�����d�l)
 *
 *	@param	UI_TEMPLATE_CLWK_SET *wk
 *	@param	tex_idx
 *	@param  ptn_ofs
 *	@param	sx
 *	@param	sy
 *	@param	GFL_CLUNIT *unit	CLUNIT
 *	@param	heap_id						�q�[�vID
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_OAM_MAPMODEL_CreateCLWK( UI_TEMPLATE_CLWK_SET *wk, u16 tex_idx, u8 ptn_ofs, GFL_CLUNIT *unit, HEAPID heap_id )
{	
  CLWK_SET_PARAM prm;

  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetNull4x4PltArcIdx();
  prm.ncg_id    = APP_COMMON_GetNull4x4CharArcIdx();
  prm.cell_id   = APP_COMMON_GetNull4x4CellArcIdx( APP_COMMON_MAPPING_128K );
  prm.anm_id    = APP_COMMON_GetNull4x4AnimeArcIdx( APP_COMMON_MAPPING_128K );
  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.pltt_line = PLTID_OBJ_OAM_MAPMODEL_M;
  prm.px        = 16;
  prm.py        = 32;

  UITemplate_OBJ_CreateCLWK( wk, &prm, unit, heap_id );

  // �e�N�X�`����]��
  {
    // �l���͊�{ 4 x 4
    int sx = 4;
    int sy = 4;

    CLWK_TransNSBTX( wk->clwk, ARCID_MMDL_RES, tex_idx, ptn_ofs, sx, sy, 0, prm.draw_type, heap_id );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  OAM�}�b�v���f�� �j��
 *
 *	@param	UI_TEMPLATE_CLWK_SET* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_OAM_MAPMODEL_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk )
{
  UItemplate_OBJ_DeleteCLWK( wk );
}

#endif // UI_TEMPLATE_OAM_MAPMODEL


#ifdef	UI_TEMPLATE_PRINT_TOOL
//=============================================================================
/**
 *	PRINT_TOOL
 */
//=============================================================================
/*
		�E�uHP ??/??�v��\������T���v���ł�
		�EBMPWIN�̃T�C�Y�� 20 x 2 �ł�
		�E���݂�HP = 618, �ő�HP = 999 �Ƃ��܂�
		�E�T���v�����C���� FALSE ��Ԃ��ƏI���ł�
*/

// �T���v�����C��
static BOOL PrintTool_MainFunc( UI_TEMPLATE_MAIN_WORK * wk )
{
	switch( wk->seq ){
	case 0:
		PrintTool_AddBmpWin( wk );			// BMPWIN�쐬
		PrintTool_PrintHP( wk );				// �g�o�\��
		PrintTool_ScreenTrans( wk );		// �X�N���[���]��
		wk->seq = 1;
		break;

	case 1:
		// �v�����g�I���҂�
		if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
			wk->seq = 2;
		}
		break;

	case 2:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
			PrintTool_DelBmpWin( wk );		// BMPWIN�폜
			return FALSE;
		}
	}

	PRINT_UTIL_Trans( &wk->print_util, wk->que );
	return TRUE;
}

// BMPWIN�쐬
static void PrintTool_AddBmpWin( UI_TEMPLATE_MAIN_WORK * wk )
{
	wk->print_util.win = GFL_BMPWIN_Create(
													GFL_BG_FRAME0_M,					// �a�f�t���[��
													1, 1,											// �\�����W
													20, 2,										// �\���T�C�Y
													15,												// �p���b�g
													GFL_BMP_CHRAREA_GET_B );	// �L�����擾����
}

// BMPWIN�폜
static void PrintTool_DelBmpWin( UI_TEMPLATE_MAIN_WORK * wk )
{
		GFL_BMPWIN_Delete( wk->print_util.win );
}

// BMPWIN�X�N���[���]��
static void PrintTool_ScreenTrans( UI_TEMPLATE_MAIN_WORK * wk )
{
	GFL_BMPWIN_MakeScreen( wk->print_util.win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(wk->print_util.win) );
}

// �g�o�\��
static void PrintTool_PrintHP( UI_TEMPLATE_MAIN_WORK * wk )
{
	STRBUF * strbuf;

	// BMPWIN���̍��W(32,0)����ɉE�l�߂Łu�g�o�v�ƕ\��
	PRINTTOOL_Print(
				&wk->print_util,				// �v�����g���[�e�B�� ( BMPWIN )
				wk->que,								// �v�����g�L���[
				32, 0,									// �\������W
				strbuf,									// ������i���łɁu�g�o�v���W�J����Ă�����̂Ƃ���j
				wk->font,								// �t�H���g
				PRINTTOOL_MODE_RIGHT );	// �E�l

	// BMPWIN���̍��W(100,0)���u�^�v�̒��S�ɂ��Ăg�o��\�� ( hp / mhp )
	PRINTTOOL_PrintFraction(
				&wk->print_util,				// �v�����g���[�e�B�� ( BMPWIN )
				wk->que,								// �v�����g�L���[
				wk->font,								// �t�H���g
				100, 0,									// �\������W
				618,										// hp
				999,										// mhp
				wk->heapID );						// �q�[�v�h�c
}
#endif	// UI_TEMPLATE_PRINT_TOOL


