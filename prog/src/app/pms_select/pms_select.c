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

#include "savedata/save_control.h"

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

#include "ui/ui_scene.h"

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

//SE
#include "sound/pm_sndsys.h"
#include "pms_select_sound_def.h"

// ���͉��
#include "app/pms_input.h"

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
  PMS_SELECT_HEAP_SIZE = 0x20000,  ///< �q�[�v�T�C�Y
};

//--------------------------------------------------------------
///	�V�[�P���X
//==============================================================

//--------------------------------------------------------------
///	SceneFunc
//==============================================================
// �v���[�g�I��
static BOOL ScenePlateSelect_Init( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL ScenePlateSelect_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL ScenePlateSelect_End( UI_SCENE_CNT_PTR cnt, void* work );
// �R�}���h�I��
static BOOL SceneCmdSelect_Init( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneCmdSelect_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneCmdSelect_End( UI_SCENE_CNT_PTR cnt, void* work );
// ���͉�ʌĂяo�������A
static BOOL SceneCallEdit_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneCallEdit_End( UI_SCENE_CNT_PTR cnt, void* work );

//--------------------------------------------------------------
///	SceneID
//==============================================================
typedef enum
{ 
  PMSS_SCENE_ID_PLATE_SELECT = 0, ///< �v���[�g�I��
  PMSS_SCENE_ID_CMD_SELECT,       ///< ���j���[
  PMSS_SCENE_ID_CALL_EDIT,        ///< ���͉�ʌĂяo�������A

  PMSS_SCENE_ID_MAX,
} PMSS_SCENE;

//--------------------------------------------------------------
///	SceneTable
//==============================================================
static const UI_SCENE_FUNC_SET c_scene_func_tbl[ PMSS_SCENE_ID_MAX ] = 
{ 
  // PMSS_SCENE_ID_PLATE_SELECT 
  {
    ScenePlateSelect_Init, 
    NULL,
    ScenePlateSelect_Main, 
    NULL, 
    ScenePlateSelect_End,
  },
  // PMSS_SCENE_ID_CMD_SELECT
  {
    SceneCmdSelect_Init,
    NULL,
    SceneCmdSelect_Main, 
    NULL,
    SceneCmdSelect_End,
  },
  // PMSS_SCENE_ID_CALL_EDIT
  {
    NULL, NULL,
    SceneCallEdit_Main,
    NULL, 
    SceneCallEdit_End,
  },
};

//--------------------------------------------------------------
///	�^�X�N���j���[ID
//==============================================================
enum
{ 
  TASKMENU_ID_DECIDE,
  TASKMENU_ID_CALL_EDIT,
  TASKMENU_ID_CANCEL,
};

//-------------------------------------
///	�t���[��
//=====================================
enum
{	
  // MAIN
	BG_FRAME_MENU_M	= GFL_BG_FRAME0_M,
	BG_FRAME_BAR_M	= GFL_BG_FRAME1_M,
	BG_FRAME_TEXT_M	= GFL_BG_FRAME2_M,
	BG_FRAME_BACK_M	= GFL_BG_FRAME3_M,
  // SUB
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
  PLTID_OBJ_PMS_DRAW    = 3, // 5�{�g�p 
  PLTID_OBJ_BALLICON_M  = 13, // 1�{�g�p
	PLTID_OBJ_TOWNMAP_M	  = 14,		
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
} PMS_SELECT_BG_WORK;


#ifdef PMS_SELECT_PMSDRAW
#define PMS_SELECT_PMSDRAW_NUM (4) ///< �ȈՉ�b�̌�
#endif // PMS_SELECT_PMSDRAW 

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct 
{
  //[IN]
  PMS_SELECT_PARAM* out_param;

  //[PRIVATE]
  HEAPID heapID;

	PMS_SELECT_BG_WORK				wk_bg;

	//�`��ݒ�
	PMS_SELECT_GRAPHIC_WORK	*graphic;

#ifdef PMS_SELECT_TOUCHBAR
	//�^�b�`�o�[
	TOUCHBAR_WORK							*touchbar;
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

#ifdef PMS_SELECT_PMSDRAW
  GFL_BMPWIN*               pms_win[ PMS_SELECT_PMSDRAW_NUM ];
  PMS_DRAW_WORK*            pms_draw;
#endif //PMS_SELECT_PMSDRAW

  UI_SCENE_CNT_PTR          cntScene;
  void*                     subproc_work;   ///< �T�uPROC�p���[�N�ۑ��̈�

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
static void PMSSelect_GRAPHIC_Load( PMS_SELECT_MAIN_WORK* wk );
static void PMSSelect_GRAPHIC_UnLoad( PMS_SELECT_MAIN_WORK* wk );
static void PMSSelect_BG_LoadResource( PMS_SELECT_BG_WORK* wk, HEAPID heapID );

#ifdef PMS_SELECT_TOUCHBAR
//-------------------------------------
///	�^�b�`�o�[
//=====================================
static void PMSSelect_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar );
//�ȉ��J�X�^���{�^���g�p�T���v���p
static TOUCHBAR_WORK * PMSSelect_TOUCHBAR_Init( PMS_SELECT_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID );
static void PMSSelect_TOUCHBAR_Exit( PMS_SELECT_MAIN_WORK *wk );
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
	
	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	//���b�Z�[�W
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_pms_input_dat, wk->heapID );

	//PRINT_QUE�쐬
	wk->print_que		= PRINTSYS_QUE_Create( wk->heapID );

  PMSSelect_GRAPHIC_Load( wk );

  // SCENE
	wk->cntScene = UI_SCENE_CNT_Create( wk->heapID, c_scene_func_tbl, PMSS_SCENE_ID_MAX, wk );

	//@TODO	�t�F�[�h�V�[�P���X���Ȃ��̂�
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

  PMSSelect_GRAPHIC_UnLoad( wk );

	//���b�Z�[�W�j��
	GFL_MSG_Delete( wk->msg );

	//PRINT_QUE
	PRINTSYS_QUE_Delete( wk->print_que );

	//FONT
	GFL_FONT_Delete( wk->font );

  // SCENE
  UI_SCENE_CNT_Delete( wk->cntScene );

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
	
  // SCENE
  if( UI_SCENE_CNT_Main( wk->cntScene ) )
  {
    return GFL_PROC_RES_FINISH;
  }

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
 *	@brief  �O���t�B�b�N ���[�h
 *	
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void PMSSelect_GRAPHIC_Load( PMS_SELECT_MAIN_WORK* wk )
{ 
	//�`��ݒ菉����
	wk->graphic	= PMS_SELECT_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heapID );

	//BG���\�[�X�ǂݍ���
	PMSSelect_BG_LoadResource( &wk->wk_bg, wk->heapID );

#ifdef PMS_SELECT_TOUCHBAR
	//�^�b�`�o�[�̏�����
	{	
		GFL_CLUNIT	*clunit	= PMS_SELECT_GRAPHIC_GetClunit( wk->graphic );
		wk->touchbar	= PMSSelect_TOUCHBAR_Init( wk, clunit, wk->heapID );
	}
#endif //PMS_SELECT_TOUCHBAR

#ifdef PMS_SELECT_TASKMENU
	//TASKMENU���\�[�X�ǂݍ��݁�������
	wk->menu_res	= APP_TASKMENU_RES_Create( BG_FRAME_MENU_M, PLTID_BG_TASKMENU_M, wk->font, wk->print_que, wk->heapID );
#endif //PMS_SELECT_TASKMENU

#ifdef PMS_SELECT_PMSDRAW  
  PMSSelect_PMSDRAW_Init( wk );
#endif //PMS_SELECT_PMSDRAW
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �O���t�B�b�N �A�����[�h
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void PMSSelect_GRAPHIC_UnLoad( PMS_SELECT_MAIN_WORK* wk )
{

#ifdef PMS_SELECT_TOUCHBAR
	//�^�b�`�o�[
	PMSSelect_TOUCHBAR_Exit( wk );
#endif //PMS_SELECT_TOUCHBAR

#ifdef PMS_SELECT_TASKMENU
	//TASKMENU�V�X�e�������\�[�X�j��
	APP_TASKMENU_RES_Delete( wk->menu_res );	
#endif //PMS_SELECT_TASKMENU

#ifdef PMS_SELECT_PMSDRAW
  PMSSelect_PMSDRAW_Exit( wk );
#endif //PMS_SELECT_PMSDRAW

	//�`��ݒ�j��
	PMS_SELECT_GRAPHIC_Exit( wk->graphic );
}
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
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_pmsi_pms2_bg_back_NCGR,
						BG_FRAME_BACK_M, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_pmsi_pms2_bg_back01_NSCR,
						BG_FRAME_BACK_M, 0, 0, 0, heapID );
	
  GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_pmsi_pms2_bg_back_NCGR,
						BG_FRAME_TEXT_M, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_pmsi_pms2_bg_back01_NSCR,
						BG_FRAME_TEXT_M, 0, 0, 0, heapID );

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
 * @param  wk										���[�N
 *	@param	GFL_CLUNIT *clunit	CLUNIT
 *	@param	heapID							�q�[�vID
 *
 *	@return	TOUCHBAR_WORK
 */
//-----------------------------------------------------------------------------
static TOUCHBAR_WORK * PMSSelect_TOUCHBAR_Init( PMS_SELECT_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID )
{	
	//�^�b�`�o�[�̐ݒ�
	
  //�A�C�R���̐ݒ�
	//�������
	TOUCHBAR_SETUP	touchbar_setup = {0};

	TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]	=
	{	
		{	
			TOUCHBAR_ICON_RETURN,
			{	TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
		},
	};

	//�ݒ�\����
	//�����قǂ̑����{���\�[�X���������
	touchbar_setup.p_item		= touchbar_icon_tbl;				//��̑����
	touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);//�����������邩
	touchbar_setup.p_unit		= clunit;										//OBJ�ǂݍ��݂̂��߂�CLUNIT
	touchbar_setup.bar_frm	= BG_FRAME_BAR_M;						//BG�ǂݍ��݂̂��߂�BG��
	touchbar_setup.bg_plt		= PLTID_BG_TOUCHBAR_M;			//BG��گ�
	touchbar_setup.obj_plt	= PLTID_OBJ_TOUCHBAR_M;			//OBJ��گ�
	touchbar_setup.mapping	= APP_COMMON_MAPPING_64K;	//�}�b�s���O���[�h

	return TOUCHBAR_Init( &touchbar_setup, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR�j��	�J�X�^���{�^����
 *
 *	@param	TOUCHBAR_WORK	*touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void PMSSelect_TOUCHBAR_Exit( PMS_SELECT_MAIN_WORK *wk )
{	
	//�^�b�`�o�[�j��
	TOUCHBAR_Exit( wk->touchbar );
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

  static u8 strtbl[] = {
    str_decide,
    str_edit,
    str_cancel,
  };

	//���̐ݒ�
	for( i = 0; i < NELEMS(item); i++ )
	{	
		item[i].str	      = GFL_MSG_CreateString( msg, strtbl[i] );	//������
		item[i].msgColor	= APP_TASKMENU_ITEM_MSGCOLOR;	//�����F
		item[i].type			= APP_TASKMENU_WIN_TYPE_NORMAL;	//���̎��
	}
	
  // �u���ǂ�v�L��
  item[2].type  = APP_TASKMENU_WIN_TYPE_RETURN,	//���ǂ�L�������������i�����j������Ȃ�

	//������
	init.heapId		= heapID;
	init.itemNum	= NELEMS(item);
	init.itemWork = item;
	init.posType	= ATPT_RIGHT_DOWN;
	init.charPosX	= 32;
	init.charPosY = 24;
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

  wk->pms_draw  = PMS_DRAW_Init( clunit, CLSYS_DRAW_MAIN, wk->print_que, wk->font, 
      PLTID_OBJ_PMS_DRAW, PMS_SELECT_PMSDRAW_NUM ,wk->heapID );
  
  {
    int i;
    PMS_DATA pms;

    // PMS�\���pBMPWIN����
    for( i=0; i<PMS_SELECT_PMSDRAW_NUM; i++ )
    {
      wk->pms_win[i] = GFL_BMPWIN_Create(
          BG_FRAME_TEXT_M,					// �a�f�t���[��
          2, 1 + 6 * i,			      	// �\�����W(�L�����P��)
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
    
    // 4�� �f�R����\��
    PMSDAT_SetDebugRandomDeco( &pms, wk->heapID );
    PMS_DRAW_Print( wk->pms_draw, wk->pms_win[3], &pms ,3 );
    
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

//=============================================================================
// SCENE
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�I�� ������
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL ScenePlateSelect_Init( UI_SCENE_CNT_PTR cnt, void* work )
{
  PMS_SELECT_MAIN_WORK* wk = work;

  // �S�ĕ\��
  TOUCHBAR_SetVisibleAll( wk->touchbar, TRUE );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�I���又��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL ScenePlateSelect_Main( UI_SCENE_CNT_PTR cnt, void* work )
{ 
  PMS_SELECT_MAIN_WORK* wk = work;

  // �f�o�b�O�{�^���ŃA�v���I��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG )
  {
    UI_SCENE_CNT_SetNextScene( cnt, UI_SCENE_ID_END );
  }

	//�^�b�`�o�[���C������
	PMSSelect_TOUCHBAR_Main( wk->touchbar );

  // �^�b�`�o�[����
  switch( TOUCHBAR_GetTrg( wk->touchbar ) )
  {
  case TOUCHBAR_ICON_RETURN :
    UI_SCENE_CNT_SetNextScene( cnt, UI_SCENE_ID_END );
    return TRUE;

  // @TODO ���E�ƍ��ڃA�C�R��

  case TOUCHBAR_ICON_CUTSOM1 :
  case TOUCHBAR_SELECT_NONE :
    break;

  default : GF_ASSERT(0);
  }
    
  // @TODO �Ƃ肠����X�Ń��j���[���J��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
  {
    UI_SCENE_CNT_SetNextScene( cnt, PMSS_SCENE_ID_CMD_SELECT );
    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�I�� �㏈��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL ScenePlateSelect_End( UI_SCENE_CNT_PTR cnt, void* work )
{
  PMS_SELECT_MAIN_WORK* wk = work;
  
  // �S�ď���
  TOUCHBAR_SetVisibleAll( wk->touchbar, FALSE );

  return TRUE;
}
  
//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneCmdSelect_Init( UI_SCENE_CNT_PTR cnt, void* work )
{
  PMS_SELECT_MAIN_WORK* wk = work;

  wk->menu = PMSSelect_TASKMENU_Init( wk->menu_res, wk->msg, wk->heapID );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�ɑ΂���R�}���h�I��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneCmdSelect_Main( UI_SCENE_CNT_PTR cnt, void* work )
{
  PMS_SELECT_MAIN_WORK* wk = work;
	
  //�^�X�N���j���[���C������
	PMSSelect_TASKMENU_Main( wk->menu );

  if( APP_TASKMENU_IsFinish( wk->menu ) )
  {
    u8 pos = APP_TASKMENU_GetCursorPos( wk->menu );

    switch( pos )
    {
    case TASKMENU_ID_DECIDE :
      // �����Ă� �� �I��
      UI_SCENE_CNT_SetNextScene( cnt, UI_SCENE_ID_END );
      break;
    case TASKMENU_ID_CALL_EDIT :
      // �ւ񂵂イ �� ���͉�ʌĂяo��
      UI_SCENE_CNT_SetNextScene( cnt, PMSS_SCENE_ID_CALL_EDIT );
      break;
    case TASKMENU_ID_CANCEL :
      // �L�����Z�� �� �I���ɖ߂�
      UI_SCENE_CNT_SetNextScene( cnt, PMSS_SCENE_ID_PLATE_SELECT );
      break;
    default : GF_ASSERT(0);
    }

    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneCmdSelect_End( UI_SCENE_CNT_PTR cnt, void* work )
{ 
  PMS_SELECT_MAIN_WORK* wk = work;

	PMSSelect_TASKMENU_Exit( wk->menu );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ւ񂵂イ���ȈՉ�b����
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneCallEdit_Main( UI_SCENE_CNT_PTR cnt, void* work )
{
  PMSI_PARAM* pmsi;
  PMS_SELECT_MAIN_WORK* wk = work;

  PMSSelect_GRAPHIC_UnLoad( wk );

  //@TODO �Z�[�u�|�C���^�O���[�o���Q��
  pmsi = PMSI_PARAM_Create(PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, SaveControl_GetPointer(), wk->heapID );

  // PROC�ؑ� ���͉�ʌĂяo��
  GFL_PROC_SysCallProc( NO_OVERLAY_ID, &ProcData_PMSInput, pmsi );
  wk->subproc_work = pmsi;
 
  HOSAKA_Printf("call! \n");

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneCallEdit_End( UI_SCENE_CNT_PTR cnt, void* work )
{ 
  PMS_SELECT_MAIN_WORK* wk = work;
  u8 seq = UI_SCENE_CNT_GetSubSeq( cnt );

  switch( seq )
  {
  case 0:
    HOSAKA_Printf("return! \n");
    PMSI_PARAM_Delete( wk->subproc_work );
    PMSSelect_GRAPHIC_Load( wk );

    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0);
    UI_SCENE_CNT_IncSubSeq( cnt );
    break;

  case 1:
    // �t�F�[�h�҂�
    if( GFL_FADE_CheckFade() == FALSE )
    {
      // ���̃V�[�����Z�b�g
      UI_SCENE_CNT_SetNextScene( cnt, PMSS_SCENE_ID_PLATE_SELECT );
      return TRUE;
    }
    break;

  default : GF_ASSERT(0);
  }

  return FALSE;
}


