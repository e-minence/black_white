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

// �ȈՉ�b�\���V�X�e��
#include "system/pms_draw.h"

//�e�N�X�`����OAM�ɓ]��
#include "system/nsbtx_to_clwk.h"

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

//�Ȉ�CLWK�ǂݍ��݁��J�����[�e�B���e�B�[
#include "ui/ui_easy_clwk.h"

//�}�b�v���f����nsbtx
#include "fieldmap/fldmmdl_mdlres.naix"

//�^�b�`�o�[
#include "ui/touchbar.h"

//�|�P�p���A�|�P����NO
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

//�|�P�A�C�R��
#include "pokeicon/pokeicon.h"

//MCSS
#include "system/mcss.h"
#include "system/mcss_tool.h"

//�^�X�N���j���[
#include "app/app_taskmenu.h"

//�ǂ����A�C�R��
#include "item/item.h"
#include "item_icon.naix"

//�|�P����BG,OBJ�ǂ݂���
#include "system/poke2dgra.h"

//�A�v�����ʑf��
#include "app/app_menu_common.h"

// �T�E���h
#include "sound/pm_sndsys.h"

//�A�[�J�C�u
#include "arc_def.h"

//�O�����J
#include "ui_template.h"


//@TODO BG�ǂݍ��� �Ƃ肠�����}�C�N�e�X�g�̃��\�[�X
#include "message.naix"
#include "mictest.naix"	// �A�[�J�C�u
#include "msg/msg_mictest.h"  // GMM
#include "townmap_gra.naix"		// �^�b�`�o�[�J�X�^���{�^���p�T���v���Ƀ^�E���}�b�v���\�[�X

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================
#define UI_TEMPLATE_INFOWIN
#define UI_TEMPLATE_TOUCHBAR
#define UI_TEMPLATE_MCSS
#define UI_TEMPLATE_TASKMENU
#define UI_TEMPLATE_TYPEICON      // ���ށA�Z�A�C�R��
#define UI_TEMPLATE_ITEM_ICON     // �ǂ����A�C�R��
#define UI_TEMPLATE_POKE_ICON     // �|�P�A�C�R��(�|�P�A�C�R���p�A�C�e���A�C�R������)
#define UI_TEMPLATE_OAM_MAPMODEL  // �}�b�v���f����OAM�ŕ\��
#define UI_TEMPLATE_POKE2D				// �|�P����BG,OBJ�ǂ݂���
#define UI_TEMPLATE_BALL					// �{�[���A�C�R���ǂ݂���
#define UI_TEMPLATE_PMSDRAW       // �ȈՉ�b�\��

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  UI_TEMPLATE_HEAP_SIZE = 0x30000,  ///< �q�[�v�T�C�Y
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
  PLTID_OBJ_PMS_DRAW = 0, // 5�{�g�p
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


#ifdef UI_TEMPLATE_PMSDRAW
#define UI_TEMPLATE_PMSDRAW_NUM (3) ///< �ȈՉ�b�̌�
#endif // UI_TEMPLATE_PMSDRAW 

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heapID;

	UI_TEMPLATE_BG_WORK				wk_bg;

	//�`��ݒ�
	UI_TEMPLATE_GRAPHIC_WORK	*graphic;

#ifdef UI_TEMPLATE_TOUCHBAR
	//�^�b�`�o�[
	TOUCHBAR_WORK							*touchbar;
	//�ȉ��J�X�^���{�^���g�p����ꍇ�̃T���v�����\�[�X
	u32												ncg_btn;
	u32												ncl_btn;
	u32												nce_btn;
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_MCSS
	//MCSS
	MCSS_SYS_WORK							*mcss_sys;
	MCSS_WORK									*mcss_wk;
#endif //UI_TEMPLATE_MCSS

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
  UI_EASY_CLWK_RES      clres_type_icon;
  GFL_CLWK                  *clwk_type_icon;
#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_ITEM_ICON
  // �ǂ����A�C�R��
  UI_EASY_CLWK_RES      clres_item_icon;
  GFL_CLWK                  *clwk_item_icon;
#endif //UI_TEMPLATE_ITEM_ICON

#ifdef UI_TEMPLATE_POKE_ICON
  // �|�P�A�C�R��
  UI_EASY_CLWK_RES      clres_poke_icon;
  GFL_CLWK                  *clwk_poke_icon;
  // �|�P�A�C�R���p�A�C�e���A�C�R��
  UI_EASY_CLWK_RES      clres_poke_item;
  GFL_CLWK                  *clwk_poke_item;
#endif //UI_TEMPLATE_POKE_ICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
  // OAM�}�b�v���f��
  UI_EASY_CLWK_RES      clres_oam_mmdl;
  GFL_CLWK                  *clwk_oam_mmdl;
#endif //UI_TEMPLATE_OAM_MAPMODEL

#ifdef UI_TEMPLATE_POKE2D
	u32												ncg_poke2d;
	u32												ncl_poke2d;
	u32												nce_poke2d;
	GFL_CLWK									*clwk_poke2d;
#endif //UI_TEMPLATE_POKE2D

#ifdef UI_TEMPLATE_BALL
  // �{�[���A�C�R��
  UI_EASY_CLWK_RES      clres_ball;
  GFL_CLWK                  *clwk_ball;
#endif //UI_TEMPLATE_BALL

#ifdef	UI_TEMPLATE_PRINT_TOOL
	//�v�����g���[�e�B���e�B
	PRINT_UTIL								print_util;
	u32												seq;
#endif	//UI_TEMPLATE_PRINT_TOOL

#ifdef UI_TEMPLATE_PMSDRAW
  GFL_BMPWIN*               pms_win[ UI_TEMPLATE_PMSDRAW_NUM ];
  PMS_DRAW_WORK*            pms_draw;
#endif //UI_TEMPLATE_PMSDRAW

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
static void UITemplate_BG_LoadResource( UI_TEMPLATE_BG_WORK* wk, HEAPID heapID );

#ifdef UI_TEMPLATE_INFOWIN
//-------------------------------------
///	INFOWIN
//=====================================
static void UITemplate_INFOWIN_Init( GAMESYS_WORK *gamesys, HEAPID heapID );
static void UITemplate_INFOWIN_Exit( void );
static void UITemplate_INFOWIN_Main( void );
#endif //UI_TEMPLATE_INFOWIN

#ifdef UI_TEMPLATE_TOUCHBAR
//-------------------------------------
///	�^�b�`�o�[
//=====================================
static TOUCHBAR_WORK * UITemplate_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heapID );
static void UITemplate_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar );
static void UITemplate_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar );
//�ȉ��J�X�^���{�^���g�p�T���v���p
static TOUCHBAR_WORK * UITemplate_TOUCHBAR_InitEx( UI_TEMPLATE_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID );
static void UITemplate_TOUCHBAR_ExitEx( UI_TEMPLATE_MAIN_WORK *wk );
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_MCSS
//-------------------------------------
///	MCSS
//=====================================
static MCSS_SYS_WORK * UITemplate_MCSS_Init( u16 wk_max, HEAPID heapID );
static void UITemplate_MCSS_Exit( MCSS_SYS_WORK * mcss );
static void UITemplate_MCSS_Draw( MCSS_SYS_WORK * mcss );
static MCSS_WORK * UITemplate_MCSS_CreateWkPP( MCSS_SYS_WORK * mcss, POKEMON_PARAM *pp, const VecFx32 *pos );
static void UITemplate_MCSS_DeleteWk( MCSS_SYS_WORK * mcss, MCSS_WORK *wk );
#endif //UI_TEMPLATE_MCSS

#ifdef UI_TEMPLATE_TASKMENU
//-------------------------------------
///	���X�g�V�X�e���͂��A������
//=====================================
static APP_TASKMENU_WORK * UITemplate_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heapID );
static void UITemplate_TASKMENU_Exit( APP_TASKMENU_WORK *menu );
static void UITemplate_TASKMENU_Main( APP_TASKMENU_WORK *menu );
#endif //UI_TEMPLATE_TASKMENU

#ifdef UI_TEMPLATE_TYPEICON
//-------------------------------------
///	���ށA�Z�A�C�R��
//=====================================
static void UITemplate_TYPEICON_CreateCLWK( UI_TEMPLATE_MAIN_WORK *wk, PokeType type, GFL_CLUNIT *unit, HEAPID heapID );
static void UITemplate_TYPEICON_DeleteCLWK( UI_TEMPLATE_MAIN_WORK *wk );
#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_ITEM_ICON
//-------------------------------------
///	�ǂ����A�C�R��
//=====================================
static void UITemplate_ITEM_ICON_CreateCLWK( UI_TEMPLATE_MAIN_WORK* wk, u16 item_id, GFL_CLUNIT* unit, HEAPID heapID );
static void UITemplate_ITEM_ICON_DeleteCLWK( UI_TEMPLATE_MAIN_WORK* wk );
#endif //UI_TEMPLATE_ITEM_ICON

#ifdef UI_TEMPLATE_POKE_ICON
//-------------------------------------
///	�|�P�A�C�R��
//=====================================
static void UITemplate_POKE_ICON_CreateCLWK( UI_TEMPLATE_MAIN_WORK* wk, u32 mons, u32 form_no, BOOL egg, GFL_CLUNIT* unit, HEAPID heapID );
static void UITemplate_POKE_ICON_DeleteCLWK( UI_TEMPLATE_MAIN_WORK* wk );
#endif //UI_TEMPLATE_POKE_ICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
//-------------------------------------
///	OAM�Ń}�b�v���f���\��
//=====================================
static void UITemplate_OAM_MAPMODEL_CreateCLWK( UI_TEMPLATE_MAIN_WORK *wk, u16 tex_idx, u8 ptn_ofs, GFL_CLUNIT *unit, HEAPID heapID );
static void UITemplate_OAM_MAPMODEL_DeleteCLWK( UI_TEMPLATE_MAIN_WORK* wk );
#endif //UI_TEMPLATE_OAM_MAPMODEL

#ifdef UI_TEMPLATE_POKE2D
//-------------------------------------
///	�|�P����OBJ,BG�ǂ݂���
//=====================================
static void UITemplate_POKE2D_LoadResourceBG( UI_TEMPLATE_MAIN_WORK *wk, HEAPID heapID );
static void UITemplate_POKE2D_UnLoadResourceBG( UI_TEMPLATE_MAIN_WORK *wk );
static void UITemplate_POKE2D_LoadResourceOBJ( UI_TEMPLATE_MAIN_WORK *wk, HEAPID heapID );
static void UITemplate_POKE2D_UnLoadResourceOBJ( UI_TEMPLATE_MAIN_WORK *wk );
static void UITemplate_POKE2D_CreateCLWK( UI_TEMPLATE_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID );
static void UITemplate_POKE2D_DeleteCLWK( UI_TEMPLATE_MAIN_WORK *wk );
#endif //UI_TEMPLATE_POKE2D

#ifdef UI_TEMPLATE_BALL
//-------------------------------------
///	�{�[���A�C�R��
//=====================================
static void UITemplate_BALLICON_CreateCLWK( UI_TEMPLATE_MAIN_WORK *wk, BALL_ID ballID, GFL_CLUNIT *unit, HEAPID heapID );
static void UITemplate_BALLICON_DeleteCLWK( UI_TEMPLATE_MAIN_WORK* wk );
#endif //UI_TEMPLATE_BALL


#ifdef UI_TEMPLATE_PMSDRAW
//-------------------------------------
///	�ȈՉ�b�\��
//=====================================
static void UITemplate_PMSDRAW_Init( UI_TEMPLATE_MAIN_WORK* wk );
static void UITemplate_PMSDRAW_Exit( UI_TEMPLATE_MAIN_WORK* wk );
static void UITemplate_PMSDRAW_Proc( UI_TEMPLATE_MAIN_WORK* wk );
#endif // UI_TEMPLATE_PMSDRAW


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
  wk->heapID = HEAPID_UI_DEBUG;
	
	//�`��ݒ菉����
	wk->graphic	= UI_TEMPLATE_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heapID );

	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	//���b�Z�[�W
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
			NARC_message_mictest_dat, wk->heapID );

	//PRINT_QUE�쐬
	wk->print_que		= PRINTSYS_QUE_Create( wk->heapID );

	//BG���\�[�X�ǂݍ���
	UITemplate_BG_LoadResource( &wk->wk_bg, wk->heapID );

#ifdef UI_TEMPLATE_INFOWIN
	//INFOWIN�̏�����
	UITemplate_INFOWIN_Init( param->gamesys, wk->heapID );
#endif //UI_TEMPLATE_INFOWIN

#ifdef UI_TEMPLATE_TOUCHBAR
	//�^�b�`�o�[�̏�����
	{	
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
		wk->touchbar	= UITemplate_TOUCHBAR_InitEx( wk, clunit, wk->heapID );
	}
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_MCSS
	//MCSS������&���[�N�쐬
	wk->mcss_sys	= UITemplate_MCSS_Init( 1, wk->heapID );
	{
		POKEMON_PARAM	*pp;  
		VecFx32 pos	= { 0, -FX32_ONE*10, 0	};

		pp	= PP_Create( MONSNO_HUSIGIBANA, 0, 0, wk->heapID );
		wk->mcss_wk		= UITemplate_MCSS_CreateWkPP( wk->mcss_sys, pp, &pos );
		GFL_HEAP_FreeMemory( pp );
	}
#endif //UI_TEMPLATE_MCSS

#ifdef UI_TEMPLATE_TASKMENU
	//TASKMENU���\�[�X�ǂݍ��݁�������
	wk->menu_res	= APP_TASKMENU_RES_Create( BG_FRAME_BAR_M, PLTID_BG_TASKMENU_M, wk->font, wk->print_que, wk->heapID );
	wk->menu			= UITemplate_TASKMENU_Init( wk->menu_res, wk->msg, wk->heapID );
#endif //UI_TEMPLATE_TASKMENU

#ifdef UI_TEMPLATE_TYPEICON
	//�����A�C�R���̓ǂݍ���
	{	
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
		UITemplate_TYPEICON_CreateCLWK( wk, POKETYPE_KUSA, clunit, wk->heapID );
	}
#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
  //OAM�}�b�v���f���̓ǂݍ���
  {
    // ��l���͌��� 0=���, 1=���A�j��1, 2=���A�j��2, 3=����....
    // �g���[�i�[�Ȃǂ͈Ⴄ�\��������̂Œ��ӁB
    u16 ptn_ofs = 3;
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );

		UITemplate_OAM_MAPMODEL_CreateCLWK( wk, NARC_fldmmdl_mdlres_hero_nsbtx, ptn_ofs, clunit, wk->heapID );
  }
#endif //UI_TEMPLATE_OAM_MAPMODEL

#ifdef UI_TEMPLATE_POKE_ICON
  // �|�P�A�C�R���̓ǂݍ���
  {
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
    UITemplate_POKE_ICON_CreateCLWK( wk, MONSNO_HUSIGIDANE, 0, FALSE, clunit, wk->heapID );
  }
#endif //UI_TEMPLATE_POKE_ICON

#ifdef UI_TEMPLATE_ITEM_ICON
  // �ǂ����A�C�R���̓ǂݍ���
  { 
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
    UITemplate_ITEM_ICON_CreateCLWK( wk, ITEM_KIZUGUSURI, clunit, wk->heapID );
  }
#endif //UI_TEMPLATE_ITEM_ICON

#ifdef UI_TEMPLATE_POKE2D
	//�|�P����OBJ,BG�ǂ݂���
	UITemplate_POKE2D_LoadResourceBG( wk, wk->heapID );
	{	
		GFL_CLUNIT *clunit = UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
		UITemplate_POKE2D_LoadResourceOBJ( wk, wk->heapID );
		UITemplate_POKE2D_CreateCLWK( wk, clunit, wk->heapID );
	}
#endif //UI_TEMPLATE_POKE2D

#ifdef UI_TEMPLATE_BALL
  // �{�[���A�C�R���̓ǂݍ���
  { 
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
    UITemplate_BALLICON_CreateCLWK( wk, BALLID_MONSUTAABOORU, clunit, wk->heapID );
  }
#endif //UI_TEMPLATE_BALL

#ifdef UI_TEMPLATE_PMSDRAW  
  UITemplate_PMSDRAW_Init( wk );
#endif //UI_TEMPLATE_PMSDRAW

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

#ifdef UI_TEMPLATE_BALL
  // �ǂ����A�C�R���̔j��
   UITemplate_BALLICON_DeleteCLWK( wk );
#endif //UI_TEMPLATE_BALL

#ifdef UI_TEMPLATE_POKE2D
	UITemplate_POKE2D_DeleteCLWK( wk );
	UITemplate_POKE2D_UnLoadResourceOBJ( wk );
	UITemplate_POKE2D_UnLoadResourceBG( wk );
#endif //UI_TEMPLATE_POKE2D

#ifdef UI_TEMPLATE_TYPEICON
	//�����A�C�R���̔j��
	UITemplate_TYPEICON_DeleteCLWK( wk );
#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_ITEM_ICON
#endif //UI_TEMPLATE_ITEM_ICON

#ifdef UI_TEMPLATE_ITEM_ICON
  //�ǂ����A�C�R���̔j��
  UITemplate_ITEM_ICON_DeleteCLWK( wk );
#endif //UI_TEMPLATE_ITEM_ICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
  //OAM�}�b�v���f���̔j��
  UITemplate_OAM_MAPMODEL_DeleteCLWK( wk );
#endif //UI_TEMPLATE_OAM_MAPMODEL

#ifdef UI_TEMPLATE_POKE_ICON
   UITemplate_POKE_ICON_DeleteCLWK( wk );
#endif //UI_TEMPLATE_POKE_ICON

#ifdef UI_TEMPLATE_TASKMENU
	//TASKMENU�V�X�e�������\�[�X�j��
	UITemplate_TASKMENU_Exit( wk->menu );
	APP_TASKMENU_RES_Delete( wk->menu_res );	
#endif //UI_TEMPLATE_TASKMENU

#ifdef UI_TEMPLATE_MCSS
	//MCSS�j��
	UITemplate_MCSS_DeleteWk( wk->mcss_sys, wk->mcss_wk );
	UITemplate_MCSS_Exit( wk->mcss_sys );
#endif //UI_TEMPLATE_MCSS

#ifdef UI_TEMPLATE_TOUCHBAR
	//�^�b�`�o�[
	UITemplate_TOUCHBAR_ExitEx( wk );
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_INFOWIN
	//INFWIN
	UITemplate_INFOWIN_Exit();
#endif //UI_TEMPLATE_INFOWIN

#ifdef UI_TEMPLATE_PMSDRAW
  UITemplate_PMSDRAW_Exit( wk );
#endif //UI_TEMPLATE_PMSDRAW

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

#ifdef UI_TEMPLATE_PMSDRAW
  UITemplate_PMSDRAW_Proc( wk );
#endif //UI_TEMPLATE_PMSDRAW

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
 *	@param	heapID  �q�[�vID 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void UITemplate_BG_LoadResource( UI_TEMPLATE_BG_WORK* wk, HEAPID heapID )
{
  //@TODO �Ƃ肠�����}�C�N�e�X�g�̃��\�[�X
  enum
  {
    RES_ARC = ARCID_MICTEST_GRA,

    RES_PAL_S_SIZE = 0x20,
    RES_PAL_M_SIZE = 0x20,

    RES_PAL_S_ID = NARC_mictest_back_bg_up_NCLR,
    RES_CHR_S_ID = NARC_mictest_back_bg_up_NCGR,
    RES_SCR_S_ID = NARC_mictest_back_bg_up_NSCR,

    RES_PAL_M_ID = NARC_mictest_back_bg_down_NCLR,
    RES_CHR_M_ID = NARC_mictest_back_bg_down_NCGR,
    RES_SCR_M_ID = NARC_mictest_back_bg_down_NSCR,

  };

	ARCHANDLE	*handle;

	handle	= GFL_ARC_OpenDataHandle( RES_ARC, heapID );

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette( handle, RES_PAL_M_ID, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, RES_PAL_M_SIZE, heapID );
	GFL_ARCHDL_UTIL_TransVramPalette( handle, RES_PAL_S_ID, PALTYPE_SUB_BG,  PLTID_BG_BACK_S, RES_PAL_S_SIZE, heapID );
	
  //	----- SUB -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, RES_CHR_S_ID, BG_FRAME_BACK_S, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(      handle, RES_SCR_S_ID,	BG_FRAME_BACK_S, 0, 0, 0, heapID );	

	//	----- MAIN -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, RES_CHR_M_ID, BG_FRAME_BACK_M, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(      handle, RES_SCR_M_ID, BG_FRAME_BACK_M, 0, 0, 0, heapID );		

	GFL_ARC_CloseDataHandle( handle );
}





#ifdef UI_TEMPLATE_INFOWIN
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
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void UITemplate_INFOWIN_Init( GAMESYS_WORK *gamesys, HEAPID heapID )
{	
	GAME_COMM_SYS_PTR comm;

  GF_ASSERT( gamesys );

	comm	= GAMESYSTEM_GetGameCommSysPtr(gamesys);
	INFOWIN_Init( BG_FRAME_BAR_M, PLTID_BG_INFOWIN_M, comm, heapID );
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
#endif //UI_TEMPLATE_INFOWIN

#ifdef UI_TEMPLATE_TOUCHBAR
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
static TOUCHBAR_WORK * UITemplate_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heapID )
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
static void UITemplate_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar )
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
static TOUCHBAR_WORK * UITemplate_TOUCHBAR_InitEx( UI_TEMPLATE_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID )
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
static void UITemplate_TOUCHBAR_ExitEx( UI_TEMPLATE_MAIN_WORK *wk )
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
static void UITemplate_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Main( touchbar );
}
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_MCSS
//=============================================================================
/**
 *		MCSS
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	MCSS������
 *
 *	@param	u16 wk_max		MCSS�̃��[�N�쐬�ő吔
 *	@param	heapID				�q�[�vID
 *
 *	@return	MCSS_SYS
 */
//-----------------------------------------------------------------------------
static MCSS_SYS_WORK * UITemplate_MCSS_Init( u16 wk_max, HEAPID heapID )
{	
	MCSS_SYS_WORK *mcss;
	mcss = MCSS_Init( wk_max , heapID );
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
#endif //UI_TEMPLATE_MCSS

#ifdef UI_TEMPLATE_TASKMENU
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENU�̏�����
 *
 *	@param	menu_res	���\�[�X
 */
//-----------------------------------------------------------------------------
static APP_TASKMENU_WORK * UITemplate_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heapID )
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
#endif //UI_TEMPLATE_TASKMENU

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
 *	@param	UI_TEMPLATE_MAIN_WORK *wk ���[�N
 *	@param	PokeType					�^�C�v
 *	@param	GFL_CLUNIT *unit	CLUNIT  ���j�b�g
 *	@param	heapID						�q�[�vID
 *
 *	@return	void
 */
//-----------------------------------------------------------------------------
static void UITemplate_TYPEICON_CreateCLWK( UI_TEMPLATE_MAIN_WORK *wk, PokeType type, GFL_CLUNIT *unit, HEAPID heapID )
{	
  UI_EASY_CLWK_RES_PARAM prm;

  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetPokeTypePltArcIdx();
  prm.ncg_id    = APP_COMMON_GetPokeTypeCharArcIdx(type);
  prm.cell_id   = APP_COMMON_GetPokeTypeCellArcIdx( APP_COMMON_MAPPING_128K );
  prm.anm_id    = APP_COMMON_GetPokeTypeAnimeArcIdx( APP_COMMON_MAPPING_128K );
  prm.pltt_line = PLTID_OBJ_TYPEICON_M;
  prm.pltt_src_ofs  = 0;
  prm.pltt_src_num = 3;
  
  UI_EASY_CLWK_LoadResource( &wk->clres_type_icon, &prm, unit, heapID );

  wk->clwk_type_icon = UI_EASY_CLWK_CreateCLWK( &wk->clres_type_icon, unit, 128, 50, 0, heapID );
		
  GFL_CLACT_WK_SetPlttOffs( wk->clwk_type_icon, APP_COMMON_GetPokeTypePltOffset(type),
				CLWK_PLTTOFFS_MODE_PLTT_TOP );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����A�^�C�v�A�C�R���j��
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void UITemplate_TYPEICON_DeleteCLWK( UI_TEMPLATE_MAIN_WORK *wk )
{	
	//CLWK�j��
	GFL_CLACT_WK_Remove( wk->clwk_type_icon );
	//���\�[�X�j��
  UI_EASY_CLWK_UnLoadResource( &wk->clres_type_icon );
}

#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_ITEM_ICON
//=============================================================================
/**
 *	ITEM_ICON
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �ǂ����A�C�R������
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk	���[�N
 *	@param	item_id �A�C�e��ID
 *	@param	unit    ���j�b�g
 *	@param	heapID  �q�[�vID
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void UITemplate_ITEM_ICON_CreateCLWK( UI_TEMPLATE_MAIN_WORK* wk, u16 item_id, GFL_CLUNIT* unit, HEAPID heapID )
{	
  UI_EASY_CLWK_RES_PARAM prm;

  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
  prm.arc_id    = ARCID_ITEMICON;
  prm.pltt_id   = ITEM_GetIndex( item_id, ITEM_GET_ICON_PAL );
  prm.ncg_id    = ITEM_GetIndex( item_id, ITEM_GET_ICON_CGX );
  prm.cell_id   = NARC_item_icon_itemicon_NCER;
  prm.anm_id    = NARC_item_icon_itemicon_NANR;
  prm.pltt_line = PLTID_OBJ_ITEMICON_M;
  prm.pltt_src_ofs  = 0;
  prm.pltt_src_num = 1;
  
  UI_EASY_CLWK_LoadResource( &wk->clres_item_icon, &prm, unit, heapID );

  wk->clwk_item_icon = UI_EASY_CLWK_CreateCLWK( &wk->clres_item_icon, unit, 16, 32, 0, heapID );
}

//-----------------------------------------------------------------------------
/**
 *	@brief	�ǂ����A�C�R���j��
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void UITemplate_ITEM_ICON_DeleteCLWK( UI_TEMPLATE_MAIN_WORK* wk )
{
	//CLWK�j��
	GFL_CLACT_WK_Remove( wk->clwk_item_icon );
  //���\�[�X�J��
  UI_EASY_CLWK_UnLoadResource( &wk->clres_item_icon );
}

#endif //UI_TEMPLATE_ITEM_ICON


#ifdef UI_TEMPLATE_POKE_ICON
//=============================================================================
/**
 *	POKE_ICON
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �|�P�A�C�R�� ����
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk ���[�N
 *	@param	unit  ���j�b�g
 *	@param	heapID  �q�[�vID  
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE_ICON_CreateCLWK( UI_TEMPLATE_MAIN_WORK* wk, u32 mons, u32 form_no, BOOL egg, GFL_CLUNIT* unit, HEAPID heapID )
{
  // �|�P�A�C�R���{��
  {
    UI_EASY_CLWK_RES_PARAM prm;

    prm.draw_type = CLSYS_DRAW_MAIN;
    prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NCLR;
    prm.arc_id    = ARCID_POKEICON;
    prm.pltt_id   = POKEICON_GetPalArcIndex();
  //  prm.ncg_id    = POKEICON_GetCgxArcIndex(ppp);
    prm.ncg_id    = POKEICON_GetCgxArcIndexByMonsNumber( mons, form_no, egg );
    prm.cell_id   = POKEICON_GetCellArcIndex(); 
    prm.anm_id    = POKEICON_GetAnmArcIndex();
    prm.pltt_line = PLTID_OBJ_POKEICON_M;
    prm.pltt_src_ofs  = 0;
    prm.pltt_src_num = 3;
    
    UI_EASY_CLWK_LoadResource( &wk->clres_poke_icon, &prm, unit, heapID );

    // �A�j���V�[�P���X�Ŏw��( 0=�m��, 1=HP�ő�, 2=HP��, 3=HP��, 4=HP��, 5=��Ԉُ� )
    wk->clwk_poke_icon = UI_EASY_CLWK_CreateCLWK( &wk->clres_poke_icon, unit, 48, 32, 1, heapID );

    // ��ɃA�C�e���A�C�R����`�悷��̂ŗD��x�������Ă���
    GFL_CLACT_WK_SetSoftPri( wk->clwk_poke_icon, 1 );

    // �I�[�g�A�j�� ON
    GFL_CLACT_WK_SetAutoAnmFlag( wk->clwk_poke_icon, TRUE );

    {
      u8 pal_num = POKEICON_GetPalNum( mons, form_no, egg );
      GFL_CLACT_WK_SetPlttOffs( wk->clwk_poke_icon, pal_num, CLWK_PLTTOFFS_MODE_OAM_COLOR );
    }
  }

  // �|�P�A�C�R���p�A�C�e���A�C�R��
  {
    UI_EASY_CLWK_RES_PARAM prm;

    prm.draw_type = CLSYS_DRAW_MAIN;
    prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
    prm.arc_id    = APP_COMMON_GetArcId();
    prm.pltt_id   = APP_COMMON_GetPokeItemIconPltArcIdx();
    prm.ncg_id    = APP_COMMON_GetPokeItemIconCharArcIdx();
    prm.cell_id   = APP_COMMON_GetPokeItemIconCellArcIdx( APP_COMMON_MAPPING_128K );
    prm.anm_id    = APP_COMMON_GetPokeItemIconAnimeArcIdx( APP_COMMON_MAPPING_128K );
    prm.pltt_line = PLTID_OBJ_POKEITEM_M;
    prm.pltt_src_ofs  = 0;
    prm.pltt_src_num = 1;
    
    UI_EASY_CLWK_LoadResource( &wk->clres_poke_item, &prm, unit, heapID );

    // �A�j���V�[�P���X�Ŏw��( 0=�ǂ���, 1=���[��, 2=�{�[�� )
    // ���ʒu�����͂Ƃ肠�����̒l�ł��B
    wk->clwk_poke_item = UI_EASY_CLWK_CreateCLWK( &wk->clres_poke_item, unit, 48+1*8+4, 32+4, 2, heapID );
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief  �|�P�A�C�R�� �j��
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE_ICON_DeleteCLWK( UI_TEMPLATE_MAIN_WORK* wk )
{ 
	//CLWK�j��
	GFL_CLACT_WK_Remove( wk->clwk_poke_icon );
  //���\�[�X�J��
  UI_EASY_CLWK_UnLoadResource( &wk->clres_poke_icon );
	
  //CLWK�j��
	GFL_CLACT_WK_Remove( wk->clwk_poke_item );
  //���\�[�X�J��
  UI_EASY_CLWK_UnLoadResource( &wk->clres_poke_item );
}

#endif //UI_TEMPLATE_POKE_ICON

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
 *	@param	UI_TEMPLATE_MAIN_WORK *wk ���[�N
 *	@param	tex_idx   �e�N�X�`����ARC���C���f�b�N�X
 *	@param  ptn_ofs   �e�N�X�`���I�t�Z�b�g�i�A�j���p�^�[���̃I�t�Z�b�g�j
 *	@param	GFL_CLUNIT *unit	CLUNIT  ���j�b�g
 *	@param	heapID						�q�[�vID
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void UITemplate_OAM_MAPMODEL_CreateCLWK( UI_TEMPLATE_MAIN_WORK *wk, u16 tex_idx, u8 ptn_ofs, GFL_CLUNIT *unit, HEAPID heapID )
{	
  UI_EASY_CLWK_RES_PARAM prm;

  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetNull4x4PltArcIdx();
  prm.ncg_id    = APP_COMMON_GetNull4x4CharArcIdx();
  prm.cell_id   = APP_COMMON_GetNull4x4CellArcIdx( APP_COMMON_MAPPING_128K );
  prm.anm_id    = APP_COMMON_GetNull4x4AnimeArcIdx( APP_COMMON_MAPPING_128K );
  prm.pltt_line = PLTID_OBJ_OAM_MAPMODEL_M;
  prm.pltt_src_ofs  = 0;
  prm.pltt_src_num = 1;

  // ���\�[�X�ǂݍ���
  UI_EASY_CLWK_LoadResource( &wk->clres_oam_mmdl, &prm, unit, heapID );
  // CLWK����
  wk->clwk_oam_mmdl = UI_EASY_CLWK_CreateCLWK( &wk->clres_oam_mmdl, unit, 80, 32, 0, heapID );

  // �e�N�X�`����]��
  {
    // �l���͊�{ 4 x 4
    int sx = NSBTX_DEF_SX;
    int sy = NSBTX_DEF_SY;

    CLWK_TransNSBTX( wk->clwk_oam_mmdl, ARCID_MMDL_RES, tex_idx, ptn_ofs, sx, sy, 0, prm.draw_type, heapID );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  OAM�}�b�v���f�� �j��
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void UITemplate_OAM_MAPMODEL_DeleteCLWK( UI_TEMPLATE_MAIN_WORK* wk )
{
	//CLWK�j��
	GFL_CLACT_WK_Remove( wk->clwk_oam_mmdl );
  //���\�[�X�J��
  UI_EASY_CLWK_UnLoadResource( &wk->clres_oam_mmdl );
}

#endif //UI_TEMPLATE_OAM_MAPMODEL


#ifdef UI_TEMPLATE_POKE2D
//=============================================================================
/**
 *	�|�P����OBJ,BG�ǂ݂���
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P����BG�ǂ݂���
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk	���[�N
 *	@param	heapID										�q�[�vID
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE2D_LoadResourceBG( UI_TEMPLATE_MAIN_WORK *wk, HEAPID heapID )
{	

	POKEMON_PASO_PARAM	*ppp;
	
	//PPP�쐬
	ppp	= (POKEMON_PASO_PARAM	*)PP_Create( MONSNO_KAMEKKUSU, 0, 0, heapID );

	//1�L�����ڂɋ󔒃L���������
	GFL_BG_FillCharacter( BG_FRAME_POKE_M, 0, 1,  0 );

	//�|�P�����̊G��]��
	//1�́���1�L�����쐬�����̂ŁB
	POKE2DGRA_BG_TransResourcePPP( ppp, POKEGRA_DIR_FRONT, BG_FRAME_POKE_M,
			1, PLTID_BG_POKE_M, heapID );

	//�X�N���[���ɏ�������œ]��
	POKE2DGRA_BG_WriteScreen( BG_FRAME_POKE_M, 1, PLTID_BG_POKE_M, 32/2-12/2+12, 24/2-12/2 );
	GFL_BG_LoadScreenReq( BG_FRAME_POKE_M );

	//PPP�j��
	GFL_HEAP_FreeMemory( ppp );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P����BG�j��
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE2D_UnLoadResourceBG( UI_TEMPLATE_MAIN_WORK *wk )
{		
	GFL_BG_FillCharacterRelease( BG_FRAME_POKE_M, 1, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�ǂ݂���
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk	���[�N
 *	@param	heapID										�q�[�vID
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE2D_LoadResourceOBJ( UI_TEMPLATE_MAIN_WORK *wk, HEAPID heapID )
{	
	POKEMON_PASO_PARAM	*ppp;
	ARCHANDLE						*handle;

	//PPP�쐬
	ppp	= (POKEMON_PASO_PARAM	*)PP_Create( MONSNO_RIZAADON, 0, 0, heapID );

	//�n���h��
	handle	= POKE2DGRA_OpenHandle( heapID );
	//���\�[�X�ǂ݂���
	wk->ncg_poke2d	= POKE2DGRA_OBJ_CGR_RegisterPPP( handle, ppp, POKEGRA_DIR_FRONT, CLSYS_DRAW_MAIN, heapID );
	wk->ncl_poke2d	= POKE2DGRA_OBJ_PLTT_RegisterPPP( handle, ppp, POKEGRA_DIR_FRONT ,CLSYS_DRAW_MAIN,  PLTID_OBJ_POKE_M*0x20,  heapID );
	wk->nce_poke2d	= POKE2DGRA_OBJ_CELLANM_RegisterPPP( ppp, POKEGRA_DIR_FRONT, APP_COMMON_MAPPING_128K, CLSYS_DRAW_MAIN, heapID );
	GFL_ARC_CloseDataHandle( handle );

	//PP�j��
	GFL_HEAP_FreeMemory( ppp );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���\�[�X�j��
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE2D_UnLoadResourceOBJ( UI_TEMPLATE_MAIN_WORK *wk )
{	
	//���\�[�X�j��
	GFL_CLGRP_PLTT_Release( wk->ncl_poke2d );
	GFL_CLGRP_CGR_Release( wk->ncg_poke2d );
	GFL_CLGRP_CELLANIM_Release( wk->nce_poke2d );

}
//----------------------------------------------------------------------------
/**
 *	@brief	CLWK�쐬
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk	���[�N
 *	@param	*clunit	���j�b�g
 *	@param	heapID	�q�[�vID
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE2D_CreateCLWK( UI_TEMPLATE_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID )
{	
	GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.pos_x	= 38;
		cldata.pos_y	= 80;
		wk->clwk_poke2d	= GFL_CLACT_WK_Create( clunit, 
				wk->ncg_poke2d,
				wk->ncl_poke2d,
				wk->nce_poke2d,
				&cldata, 
				CLSYS_DEFREND_MAIN, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	CLWK�j��
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE2D_DeleteCLWK( UI_TEMPLATE_MAIN_WORK *wk )
{	
	GFL_CLACT_WK_Remove( wk->clwk_poke2d );
}
#endif //UI_TEMPLATE_POKE2D

#ifdef UI_TEMPLATE_BALL
//=============================================================================
/**
 *	�{�[���A�C�R��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�{�[���A�C�R���쐬
 *
 *	@param	UI_TEMPLATE_MAIN_WORK *wk	���[�N
 *	@param	ballID	�{�[��ID 
 *	@param	*unit		CLUNIT
 *	@param	heapID	�q�[�vID
 */
//-----------------------------------------------------------------------------
static void UITemplate_BALLICON_CreateCLWK( UI_TEMPLATE_MAIN_WORK *wk, BALL_ID ballID, GFL_CLUNIT *unit, HEAPID heapID )
{	
  UI_EASY_CLWK_RES_PARAM prm;

  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetBallPltArcIdx( ballID );
  prm.ncg_id    = APP_COMMON_GetBallCharArcIdx( ballID );
  prm.cell_id   = APP_COMMON_GetBallCellArcIdx( ballID, APP_COMMON_MAPPING_128K );
  prm.anm_id    = APP_COMMON_GetBallAnimeArcIdx( ballID, APP_COMMON_MAPPING_128K );
  prm.pltt_line = PLTID_OBJ_BALLICON_M;
  prm.pltt_src_ofs  = 0;
  prm.pltt_src_num = 1;

  // ���\�[�X�ǂݍ���
  UI_EASY_CLWK_LoadResource( &wk->clres_ball, &prm, unit, heapID );
  // CLWK����
  wk->clwk_ball = UI_EASY_CLWK_CreateCLWK( &wk->clres_ball, unit, 96, 32, 0, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�[���A�C�R���j��
 *
 *	@param	UI_TEMPLATE_MAIN_WORK* wk ���[�N
 */
//-----------------------------------------------------------------------------
static void UITemplate_BALLICON_DeleteCLWK( UI_TEMPLATE_MAIN_WORK* wk )
{	
	//CLWK�j��
	GFL_CLACT_WK_Remove( wk->clwk_ball );
  //���\�[�X�J��
  UI_EASY_CLWK_UnLoadResource( &wk->clres_ball );
}
#endif //UI_TEMPLATE_BALL

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
#endif	//UI_TEMPLATE_PRINT_TOOL

#ifdef UI_TEMPLATE_PMSDRAW

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\�� ����������
 *
 *	@param	UI_TEMPLATE_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_PMSDRAW_Init( UI_TEMPLATE_MAIN_WORK* wk )
{
	GFL_CLUNIT	*clunit;
  
  clunit = UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );

  wk->pms_draw  = PMS_DRAW_Init( clunit, CLSYS_DRAW_SUB, wk->print_que, wk->font, 
      PLTID_OBJ_PMS_DRAW, UI_TEMPLATE_PMSDRAW_NUM ,wk->heapID );
  
  {
    int i;
    PMS_DATA pms;

    // PMS�\���pBMPWIN����
    for( i=0; i<UI_TEMPLATE_PMSDRAW_NUM; i++ )
    {
      wk->pms_win[i] = GFL_BMPWIN_Create(
          BG_FRAME_TEXT_S,					// �a�f�t���[��
          2+4*i, 0 + 6 * i,					  	// �\�����W(�L�����P��)
          28, 4,    							  // �\���T�C�Y
          15,												// �p���b�g
          GFL_BMP_CHRAREA_GET_B );	// �L�����擾����
    }
    
    // 1�� �ʏ�+�󗓕\��
    PMSDAT_SetDebug( &pms );
    PMSDAT_SetWord( &pms, 1, PMS_WORD_NULL );
    PMS_DRAW_Print( wk->pms_draw, wk->pms_win[0], &pms ,0 );

    // 2�� �f�R��
    PMSDAT_SetDeco( &pms, 1, PMS_DECOID_HERO );
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
 *	@param	UI_TEMPLATE_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_PMSDRAW_Exit( UI_TEMPLATE_MAIN_WORK* wk )
{
  PMS_DRAW_Exit( wk->pms_draw );
  {
    int i;
    for( i=0; i<UI_TEMPLATE_PMSDRAW_NUM; i++ )
    {
      GFL_BMPWIN_Delete( wk->pms_win[i] );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\�� �又��
 *
 *	@param	UI_TEMPLATE_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_PMSDRAW_Proc( UI_TEMPLATE_MAIN_WORK* wk )
{
#if 1
  // SELECT�ŃN���A
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    int i;
    for( i=0; i<UI_TEMPLATE_PMSDRAW_NUM; i++ )
    {
      PMS_DRAW_Clear( wk->pms_draw, i, TRUE );
    }
  }
  // START�Ń����_���}���i��d�o�^����ƃA�T�[�g�j
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
  {
    int i;
    for( i=0; i<UI_TEMPLATE_PMSDRAW_NUM; i++ )
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

#endif // UI_TEMPLATE_PMSDRAW

