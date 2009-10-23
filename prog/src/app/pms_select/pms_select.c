//=============================================================================
/**
 *
 *	@file		pms_select.c
 *	@brief  �o�^�ς݊ȈՉ�b�I�����
 *	@author	hosaka genya
 *	@data		2009.10.20
 *
 */
//=============================================================================

//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

// �ȈՉ�b�\���V�X�e��
#include "system/pms_draw.h"

//FONT
#include "print/gf_font.h"
#include "font/font.naix"

//PRINT_QUE
#include "print/printsys.h"
#include "print/wordset.h"

//�`��ݒ�
#include "pms_select_graphic.h"

//�Ȉ�CLWK�ǂݍ��݁��J�����[�e�B���e�B�[
#include "ui/ui_easy_clwk.h"

//�^�b�`�o�[
#include "ui/touchbar.h"

//�^�X�N���j���[
#include "app/app_taskmenu.h"

//�A�v�����ʑf��
#include "app/app_menu_common.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "pmsi.naix"
#include "townmap_gra.naix"		// �^�b�`�o�[�J�X�^���{�^���p�T���v���Ƀ^�E���}�b�v���\�[�X
#include "message.naix"
#include "msg/msg_pms_input.h"

//�O�����J
#include "app/pms_select.h"

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================
#define PMS_SELECT_TOUCHBAR
#define PMS_SELECT_TASKMENU
#define PMS_SELECT_PMSDRAW       // �ȈՉ�b�\��

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  PMS_SELECT_HEAP_SIZE = 0x30000,  ///< �q�[�v�T�C�Y
};


//-------------------------------------
///	�t���[��
//=====================================
enum
{	
	BG_FRAME_BAR_M	= GFL_BG_FRAME1_M,
	BG_FRAME_POKE_M	= GFL_BG_FRAME2_M,
	BG_FRAME_BACK_M	= GFL_BG_FRAME3_M,
	BG_FRAME_BACK_S	= GFL_BG_FRAME2_S,
  BG_FRAME_TEXT_S = GFL_BG_FRAME0_S, 
};
//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	//���C��BG
	PLTID_BG_BACK_M				= 0,
	PLTID_BG_POKE_M				= 1,
	PLTID_BG_TASKMENU_M		= 11,
	PLTID_BG_TOUCHBAR_M		= 13,
	PLTID_BG_INFOWIN_M		= 15,
	//�T�uBG
	PLTID_BG_BACK_S				=	0,

	//���C��OBJ
	PLTID_OBJ_TOUCHBAR_M	= 0, // 3�{�g�p
	PLTID_OBJ_TYPEICON_M	= 3, // 3�{�g�p
  PLTID_OBJ_OAM_MAPMODEL_M = 6, // 1�{�g�p
  PLTID_OBJ_POKEICON_M = 7,     // 3�{�g�p
  PLTID_OBJ_POKEITEM_M = 10,    // 1�{�g�p
  PLTID_OBJ_ITEMICON_M = 11,
  PLTID_OBJ_POKE_M = 12,
  PLTID_OBJ_BALLICON_M = 13, // 1�{�g�p
	PLTID_OBJ_TOWNMAP_M	= 14,		
	//�T�uOBJ
  PLTID_OBJ_PMS_DRAW = 0, // 2-5�{�g�p // @TODO ����
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
} PMS_SELECT_BG_WORK;


#ifdef PMS_SELECT_PMSDRAW
#define PMS_SELECT_PMSDRAW_NUM (3) ///< �ȈՉ�b�̌�
#endif // PMS_SELECT_PMSDRAW 

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heapID;

	PMS_SELECT_BG_WORK				wk_bg;

	//�`��ݒ�
	PMS_SELECT_GRAPHIC_WORK	*graphic;

#ifdef PMS_SELECT_TOUCHBAR
	//�^�b�`�o�[
	TOUCHBAR_WORK							*touchbar;
	//�ȉ��J�X�^���{�^���g�p����ꍇ�̃T���v�����\�[�X
	u32												ncg_btn;
	u32												ncl_btn;
	u32												nce_btn;
#endif //PMS_SELECT_TOUCHBAR

	//�t�H���g
	GFL_FONT									*font;

	//�v�����g�L���[
	PRINT_QUE									*print_que;
	GFL_MSGDATA								*msg;

#ifdef PMS_SELECT_TASKMENU
	//�^�X�N���j���[
	APP_TASKMENU_RES					*menu_res;
	APP_TASKMENU_WORK					*menu;
#endif //PMS_SELECT_TASKMENU

#ifdef	PMS_SELECT_PRINT_TOOL
	//�v�����g���[�e�B���e�B
	PRINT_UTIL								print_util;
	u32												seq;
#endif	//PMS_SELECT_PRINT_TOOL

#ifdef PMS_SELECT_PMSDRAW
  GFL_BMPWIN*               pms_win[ PMS_SELECT_PMSDRAW_NUM ];
  PMS_DRAW_WORK*            pms_draw;
#endif //PMS_SELECT_PMSDRAW

} PMS_SELECT_MAIN_WORK;


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
static GFL_PROC_RESULT PMSSelectProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT PMSSelectProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT PMSSelectProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
//-------------------------------------
///	�ėp�������[�e�B���e�B
//=====================================
static void PMSSelect_BG_LoadResource( PMS_SELECT_BG_WORK* wk, HEAPID heapID );

#ifdef PMS_SELECT_TOUCHBAR
//-------------------------------------
///	�^�b�`�o�[
//=====================================
static TOUCHBAR_WORK * PMSSelect_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heapID );
static void PMSSelect_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar );
static void PMSSelect_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar );
//�ȉ��J�X�^���{�^���g�p�T���v���p
static TOUCHBAR_WORK * PMSSelect_TOUCHBAR_InitEx( PMS_SELECT_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID );
static void PMSSelect_TOUCHBAR_ExitEx( PMS_SELECT_MAIN_WORK *wk );
#endif //PMS_SELECT_TOUCHBAR

#ifdef PMS_SELECT_TASKMENU
//-------------------------------------
///	���X�g�V�X�e���͂��A������
//=====================================
static APP_TASKMENU_WORK * PMSSelect_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heapID );
static void PMSSelect_TASKMENU_Exit( APP_TASKMENU_WORK *menu );
static void PMSSelect_TASKMENU_Main( APP_TASKMENU_WORK *menu );
#endif //PMS_SELECT_TASKMENU

#ifdef PMS_SELECT_PMSDRAW
//-------------------------------------
///	�ȈՉ�b�\��
//=====================================
static void PMSSelect_PMSDRAW_Init( PMS_SELECT_MAIN_WORK* wk );
static void PMSSelect_PMSDRAW_Exit( PMS_SELECT_MAIN_WORK* wk );
static void PMSSelect_PMSDRAW_Proc( PMS_SELECT_MAIN_WORK* wk );
#endif // PMS_SELECT_PMSDRAW


//=============================================================================
/**
 *								�O�����J
 */
//=============================================================================
const GFL_PROC_DATA ProcData_PMSSelect = 
{
	PMSSelectProc_Init,
	PMSSelectProc_Main,
	PMSSelectProc_Exit,
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
 *	@param	*pwk					PMS_SELECT_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT PMSSelectProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
	PMS_SELECT_MAIN_WORK *wk;
	PMS_SELECT_PARAM *param;

	//�I�[�o�[���C�ǂݍ���
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	
	//�����擾
	param	= pwk;

	//�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_UI_DEBUG, PMS_SELECT_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(PMS_SELECT_MAIN_WORK), HEAPID_UI_DEBUG );
  GFL_STD_MemClear( wk, sizeof(PMS_SELECT_MAIN_WORK) );

  // ������
  wk->heapID = HEAPID_UI_DEBUG;
	
	//�`��ݒ菉����
	wk->graphic	= PMS_SELECT_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heapID );

	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	//���b�Z�[�W
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
			NARC_message_pms_input_dat, wk->heapID );

	//PRINT_QUE�쐬
	wk->print_que		= PRINTSYS_QUE_Create( wk->heapID );

	//BG���\�[�X�ǂݍ���
	PMSSelect_BG_LoadResource( &wk->wk_bg, wk->heapID );

#ifdef PMS_SELECT_TOUCHBAR
	//�^�b�`�o�[�̏�����
	{	
		GFL_CLUNIT	*clunit	= PMS_SELECT_GRAPHIC_GetClunit( wk->graphic );
		wk->touchbar	= PMSSelect_TOUCHBAR_InitEx( wk, clunit, wk->heapID );

    TOUCHBAR_SetActive( wk->touchbar, TOUCHBAR_ICON_CLOSE, FALSE );
	}
#endif //PMS_SELECT_TOUCHBAR

#ifdef PMS_SELECT_TASKMENU
	//TASKMENU���\�[�X�ǂݍ��݁�������
	wk->menu_res	= APP_TASKMENU_RES_Create( BG_FRAME_BAR_M, PLTID_BG_TASKMENU_M, wk->font, wk->print_que, wk->heapID );
	wk->menu			= PMSSelect_TASKMENU_Init( wk->menu_res, wk->msg, wk->heapID );
#endif //PMS_SELECT_TASKMENU

#ifdef PMS_SELECT_PMSDRAW  
  PMSSelect_PMSDRAW_Init( wk );
#endif //PMS_SELECT_PMSDRAW

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
 *	@param	*pwk					PMS_SELECT_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT PMSSelectProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	PMS_SELECT_MAIN_WORK* wk = mywk;

#ifdef PMS_SELECT_TASKMENU
	//TASKMENU�V�X�e�������\�[�X�j��
	PMSSelect_TASKMENU_Exit( wk->menu );
	APP_TASKMENU_RES_Delete( wk->menu_res );	
#endif //PMS_SELECT_TASKMENU

#ifdef PMS_SELECT_TOUCHBAR
	//�^�b�`�o�[
	PMSSelect_TOUCHBAR_ExitEx( wk );
#endif //PMS_SELECT_TOUCHBAR

#ifdef PMS_SELECT_PMSDRAW
  PMSSelect_PMSDRAW_Exit( wk );
#endif //PMS_SELECT_PMSDRAW

	//���b�Z�[�W�j��
	GFL_MSG_Delete( wk->msg );

	//PRINT_QUE
	PRINTSYS_QUE_Delete( wk->print_que );

	//FONT
	GFL_FONT_Delete( wk->font );

	//�`��ݒ�j��
	PMS_SELECT_GRAPHIC_Exit( wk->graphic );

	//PROC�p���������
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( wk->heapID );

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
 *	@param	*pwk					PMS_SELECT_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT PMSSelectProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	PMS_SELECT_MAIN_WORK* wk = mywk;

  // �f�o�b�O�{�^���ŃA�v���I��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG )
  {
    return GFL_PROC_RES_FINISH;
  }

#ifdef PMS_SELECT_TOUCHBAR
	//�^�b�`�o�[���C������
	PMSSelect_TOUCHBAR_Main( wk->touchbar );
#endif //PMS_SELECT_TOUCHBAR

#ifdef PMS_SELECT_TASKMENU
	//�^�X�N���j���[���C������
	PMSSelect_TASKMENU_Main( wk->menu );
#endif //PMS_SELECT_TASKMENU

	//PRINT_QUE
	PRINTSYS_QUE_Main( wk->print_que );

#ifdef PMS_SELECT_PMSDRAW
  PMSSelect_PMSDRAW_Proc( wk );
#endif //PMS_SELECT_PMSDRAW

	//2D�`��
	PMS_SELECT_GRAPHIC_2D_Draw( wk->graphic );

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
 *	@param	PMS_SELECT_BG_WORK* wk BG�Ǘ����[�N
 *	@param	heapID  �q�[�vID 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void PMSSelect_BG_LoadResource( PMS_SELECT_BG_WORK* wk, HEAPID heapID )
{
	ARCHANDLE	*handle;
	
  handle	= GFL_ARC_OpenDataHandle( ARCID_PMSI_GRAPHIC, heapID );

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_pmsi_pms2_bg_main_NCLR, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_pmsi_pms2_bg_main_NCLR, PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0, heapID );
	
  //	----- ���� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_pmsi_pms2_bg_back_NCGR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_pmsi_pms2_bg_back_NSCR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );

	//	----- ����� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_pmsi_pms2_bg_main_NCGR,
						BG_FRAME_BACK_M, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_pmsi_pms2_bg_main1_NSCR,
						BG_FRAME_BACK_M, 0, 0, 0, heapID );

	GFL_ARC_CloseDataHandle( handle );
}

#ifdef PMS_SELECT_TOUCHBAR
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
 *	@param	heapID							�q�[�vID
 *
 *	@return	TOUCHBAR_WORK
 */
//-----------------------------------------------------------------------------
static TOUCHBAR_WORK * PMSSelect_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heapID )
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
	GFL_STD_MemClear( &touchbar_setup, sizeof(TOUCHBAR_SETUP) );

	touchbar_setup.p_item		= touchbar_icon_tbl;				//��̑����
	touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);//�����������邩
	touchbar_setup.p_unit		= clunit;										//OBJ�ǂݍ��݂̂��߂�CLUNIT
	touchbar_setup.bar_frm	= BG_FRAME_BAR_M;						//BG�ǂݍ��݂̂��߂�BG��
	touchbar_setup.bg_plt		= PLTID_BG_TOUCHBAR_M;			//BG��گ�
	touchbar_setup.obj_plt	= PLTID_OBJ_TOUCHBAR_M;			//OBJ��گ�
	touchbar_setup.mapping	= APP_COMMON_MAPPING_128K;	//�}�b�s���O���[�h

	return TOUCHBAR_Init( &touchbar_setup, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR�j��
 *
 *	@param	TOUCHBAR_WORK	*touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void PMSSelect_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Exit( touchbar );
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR������	+ �J�X�^���A�C�R����
 *
 * @param  wk										���[�N
 *	@param	GFL_CLUNIT *clunit	CLUNIT
 *	@param	heapID							�q�[�vID
 *
 *	@return	TOUCHBAR_WORK
 */
//-----------------------------------------------------------------------------
static TOUCHBAR_WORK * PMSSelect_TOUCHBAR_InitEx( PMS_SELECT_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID )
{	

	//���\�[�X�ǂ݂���
	//�T���v���Ƃ��ă^�E���}�b�v�̊g��k���A�C�R�����J�X�^���{�^���ɓo�^����
	{	
		ARCHANDLE *handle;
		//�n���h��
		handle	= GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, heapID );
		//���\�[�X�ǂ݂���
		wk->ncg_btn	= GFL_CLGRP_CGR_Register( handle,
					NARC_townmap_gra_tmap_obj_d_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );
		wk->ncl_btn	= GFL_CLGRP_PLTT_RegisterEx( handle,
				NARC_townmap_gra_tmap_bg_d_NCLR, CLSYS_DRAW_MAIN, PLTID_OBJ_TOWNMAP_M*0x20,
				2, 2, heapID );	
		wk->nce_btn	= GFL_CLGRP_CELLANIM_Register( handle,
					NARC_townmap_gra_tmap_obj_d_NCER,
				 	NARC_townmap_gra_tmap_obj_d_NANR, heapID );
	
		GFL_ARC_CloseDataHandle( handle ) ;
	}

	//�^�b�`�o�[�̐ݒ�
	{	
	//�A�C�R���̐ݒ�
	//�������
	TOUCHBAR_SETUP	touchbar_setup;
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
			{	TOUCHBAR_ICON_X_05, TOUCHBAR_ICON_Y_CHECK }
		},
		{	
			TOUCHBAR_ICON_CUTSOM1,	//�J�X�^���{�^���P���g��k���A�C�R����,
			{	TOUCHBAR_ICON_X_04, TOUCHBAR_ICON_Y },
		},
	};

	//�ȉ��J�X�^���{�^���Ȃ�Γ���Ȃ��Ă͂����Ȃ����
	touchbar_icon_tbl[3].cg_idx	=  wk->ncg_btn;				//�L�������\�[�X
	touchbar_icon_tbl[3].plt_idx	= wk->ncl_btn;				//�p���b�g���\�[�X
	touchbar_icon_tbl[3].cell_idx	=	wk->nce_btn;				//�Z�����\�[�X
	touchbar_icon_tbl[3].active_anmseq	=	8;						//�A�N�e�B�u�̂Ƃ��̃A�j��
	touchbar_icon_tbl[3].noactive_anmseq	=		12;						//�m���A�N�e�B�u�̂Ƃ��̃A�j��
	touchbar_icon_tbl[3].push_anmseq	=		10;						//�������Ƃ��̃A�j���iSTOP�ɂȂ��Ă��邱�Ɓj
	touchbar_icon_tbl[3].key	=		PAD_BUTTON_START;		//�L�[�ŉ������Ƃ��ɓ��삳�������Ȃ�΁A�{�^���ԍ�
	touchbar_icon_tbl[3].se	=		0;									//�������Ƃ���SE�Ȃ炵�����Ȃ�΁ASE�̔ԍ�

	//�ݒ�\����
	//�����قǂ̑����{���\�[�X���������
	GFL_STD_MemClear( &touchbar_setup, sizeof(TOUCHBAR_SETUP) );

	touchbar_setup.p_item		= touchbar_icon_tbl;				//��̑����
	touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);//�����������邩
	touchbar_setup.p_unit		= clunit;										//OBJ�ǂݍ��݂̂��߂�CLUNIT
	touchbar_setup.bar_frm	= BG_FRAME_BAR_M;						//BG�ǂݍ��݂̂��߂�BG��
	touchbar_setup.bg_plt		= PLTID_BG_TOUCHBAR_M;			//BG��گ�
	touchbar_setup.obj_plt	= PLTID_OBJ_TOUCHBAR_M;			//OBJ��گ�
	touchbar_setup.mapping	= APP_COMMON_MAPPING_128K;	//�}�b�s���O���[�h

	return TOUCHBAR_Init( &touchbar_setup, heapID );
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR�j��	�J�X�^���{�^����
 *
 *	@param	TOUCHBAR_WORK	*touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void PMSSelect_TOUCHBAR_ExitEx( PMS_SELECT_MAIN_WORK *wk )
{	
	//�^�b�`�o�[�j��
	TOUCHBAR_Exit( wk->touchbar );

	//���\�[�X�j��
	{	
		GFL_CLGRP_PLTT_Release( wk->ncl_btn );
		GFL_CLGRP_CGR_Release( wk->ncg_btn );
		GFL_CLGRP_CELLANIM_Release( wk->nce_btn );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR���C������
 *
 *	@param	TOUCHBAR_WORK	*touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void PMSSelect_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Main( touchbar );
}
#endif //PMS_SELECT_TOUCHBAR

#ifdef PMS_SELECT_TASKMENU
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENU�̏�����
 *
 *	@param	menu_res	���\�[�X
 */
//-----------------------------------------------------------------------------
static APP_TASKMENU_WORK * PMSSelect_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heapID )
{	
	int i;
	APP_TASKMENU_INITWORK	init;
	APP_TASKMENU_ITEMWORK	item[3];
	APP_TASKMENU_WORK			*menu;	

	//���̐ݒ�
	for( i = 0; i < NELEMS(item); i++ )
	{	
		item[i].str	= GFL_MSG_CreateString( msg, 0 );	//������
		item[i].msgColor	= APP_TASKMENU_ITEM_MSGCOLOR;	//�����F
		item[i].type			= APP_TASKMENU_WIN_TYPE_NORMAL;	//���̎��
	}

	//������
	init.heapId		= heapID;
	init.itemNum	= NELEMS(item);
	init.itemWork = item;
	init.posType	= ATPT_RIGHT_DOWN;
	init.charPosX	= 32;
	init.charPosY = 21;
	init.w				= APP_TASKMENU_PLATE_WIDTH;
	init.h				= APP_TASKMENU_PLATE_HEIGHT;

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
static void PMSSelect_TASKMENU_Exit( APP_TASKMENU_WORK *menu )
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
static void PMSSelect_TASKMENU_Main( APP_TASKMENU_WORK *menu )
{	
	APP_TASKMENU_UpdateMenu( menu );
}
#endif //PMS_SELECT_TASKMENU

#ifdef	PMS_SELECT_PRINT_TOOL
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
static BOOL PrintTool_MainFunc( PMS_SELECT_MAIN_WORK * wk )
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
static void PrintTool_AddBmpWin( PMS_SELECT_MAIN_WORK * wk )
{
	wk->print_util.win = GFL_BMPWIN_Create(
													GFL_BG_FRAME0_M,					// �a�f�t���[��
													1, 1,											// �\�����W
													20, 2,										// �\���T�C�Y
													15,												// �p���b�g
													GFL_BMP_CHRAREA_GET_B );	// �L�����擾����
}

// BMPWIN�폜
static void PrintTool_DelBmpWin( PMS_SELECT_MAIN_WORK * wk )
{
		GFL_BMPWIN_Delete( wk->print_util.win );
}

// BMPWIN�X�N���[���]��
static void PrintTool_ScreenTrans( PMS_SELECT_MAIN_WORK * wk )
{
	GFL_BMPWIN_MakeScreen( wk->print_util.win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(wk->print_util.win) );
}

// �g�o�\��
static void PrintTool_PrintHP( PMS_SELECT_MAIN_WORK * wk )
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
#endif	//PMS_SELECT_PRINT_TOOL

#ifdef PMS_SELECT_PMSDRAW

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\�� ����������
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_PMSDRAW_Init( PMS_SELECT_MAIN_WORK* wk )
{
	GFL_CLUNIT	*clunit;
  
  clunit = PMS_SELECT_GRAPHIC_GetClunit( wk->graphic );

  wk->pms_draw  = PMS_DRAW_Init( clunit, CLSYS_DRAW_SUB, wk->print_que, wk->font, 
      PLTID_OBJ_PMS_DRAW, PMS_SELECT_PMSDRAW_NUM ,wk->heapID );
  
  {
    int i;
    PMS_DATA pms;

    // PMS�\���pBMPWIN����
    for( i=0; i<PMS_SELECT_PMSDRAW_NUM; i++ )
    {
      wk->pms_win[i] = GFL_BMPWIN_Create(
          BG_FRAME_TEXT_S,					// �a�f�t���[��
          2+4*i, 0 + 6 * i,					  	// �\�����W(�L�����P��)
          28, 4,    							  // �\���T�C�Y
          15,												// �p���b�g
          GFL_BMP_CHRAREA_GET_B );	// �L�����擾����
    }
    
    // 1��
    PMSDAT_SetDebug( &pms );
    PMS_DRAW_Print( wk->pms_draw, wk->pms_win[0], &pms ,0 );

    // 2�� �f�R���\��
    PMSDAT_SetDeco( &pms, 0, PMS_DECOID_HERO );
    PMS_DRAW_Print( wk->pms_draw, wk->pms_win[1], &pms ,1 );
    
    // 3�� �f�R����\��
    PMSDAT_SetDeco( &pms, 0, PMS_DECOID_TANKS );
    PMSDAT_SetDeco( &pms, 1, PMS_DECOID_LOVE );
    PMS_DRAW_Print( wk->pms_draw, wk->pms_win[2], &pms ,2 );
    
    // 1�̗v�f��2�ɃR�s�[(�ړ��\���Ȃǂɂ��g�p���������B)
    PMS_DRAW_Copy( wk->pms_draw, 1, 2 );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\�� �㏈��
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_PMSDRAW_Exit( PMS_SELECT_MAIN_WORK* wk )
{
  PMS_DRAW_Exit( wk->pms_draw );
  {
    int i;
    for( i=0; i<PMS_SELECT_PMSDRAW_NUM; i++ )
    {
      GFL_BMPWIN_Delete( wk->pms_win[i] );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\�� �又��
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_PMSDRAW_Proc( PMS_SELECT_MAIN_WORK* wk )
{
#if 1
  // SELECT�ŃN���A
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    int i;
    for( i=0; i<PMS_SELECT_PMSDRAW_NUM; i++ )
    {
      PMS_DRAW_Clear( wk->pms_draw, i );
    }
  }
  // START�Ń����_���}���i��d�o�^����ƃA�T�[�g�j
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
  {
    int i;
    for( i=0; i<PMS_SELECT_PMSDRAW_NUM; i++ )
    {
      PMS_DATA pms;
      PMSDAT_SetDebugRandom( &pms );
      PMSDAT_SetDeco( &pms, 0, GFUser_GetPublicRand(10)+1 );
      PMS_DRAW_Print( wk->pms_draw, wk->pms_win[i], &pms ,i );
    }
  }
#endif

  PMS_DRAW_Main( wk->pms_draw );
}

#endif // PMS_SELECT_PMSDRAW

