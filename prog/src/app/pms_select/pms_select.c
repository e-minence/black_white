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
///	BG
//==============================================================
enum
{ 
  PLATE_BG_SX = 29,
  PLATE_BG_SY = 6,
  PLATE_BG_START_PY = 1,
  PLATE_BG_PLTT_OFS_BASE = 0x2,
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
	BG_FRAME_MENU_M	  = GFL_BG_FRAME0_M,
	BG_FRAME_BAR_M	  = GFL_BG_FRAME1_M,
//	BG_FRAME_TEXT_M	= GFL_BG_FRAME1_M,
	BG_FRAME_TEXT_M	  = GFL_BG_FRAME2_M,
	BG_FRAME_BACK_M	  = GFL_BG_FRAME3_M,
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
	PLTID_BG_COMMON_M			= 0,
  PLTID_BG_MSG_M        = 7,
	PLTID_BG_TASKMENU_M		= 12,
	PLTID_BG_TOUCHBAR_M		= 14,

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
  GFL_ARCUTIL_TRANSINFO m_back_chr_pos;
  void*             plateScrnWork;
  NNSG2dScreenData* platescrnData;
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

  SAVE_CONTROL_WORK*      save_ctrl;
  PMSW_SAVEDATA*          pmsw_save;

	PMS_SELECT_BG_WORK			wk_bg;

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
  BOOL                      pms_win_tranReq[ PMS_SELECT_PMSDRAW_NUM ];
  PMS_DRAW_WORK*            pms_draw;
#endif //PMS_SELECT_PMSDRAW

  UI_SCENE_CNT_PTR          cntScene;
  void*                     subproc_work;   ///< �T�uPROC�p���[�N�ۑ��̈�
  u8    select_id;      ///< �I������ID
  u8    list_head_id;   ///< ���X�g�̐擪ID
  u8    list_max;       ///< ���X�g�̍��ڐ�
  u8    padding[1];

  BOOL bProcChange; ///< PROC�ؑփt���O

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
static void PMSSelect_BG_UnLoadResource( PMS_SELECT_BG_WORK* wk );
static void PMSSelect_BG_PlateTrans( PMS_SELECT_BG_WORK* wk, u8 view_pos_id, u32 sentence_type, BOOL is_select );
static void PMSSelect_BG_PlateFutterTrans( PMS_SELECT_BG_WORK* wk, u32 sentence_type );

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

static void PLATE_CNT_Setup( PMS_SELECT_MAIN_WORK* wk );
static void PLATE_CNT_UpdateAll( PMS_SELECT_MAIN_WORK* wk );
static void PLATE_CNT_UpdateCore( PMS_SELECT_MAIN_WORK* wk, BOOL is_check_print_end );
static void PLATE_CNT_Main( PMS_SELECT_MAIN_WORK* wk );
static void PLATE_CNT_Trans( PMS_SELECT_MAIN_WORK* wk );
static BOOL PLATE_UNIT_Trans( PRINT_QUE* print_que, GFL_BMPWIN* win, BOOL *transReq );
static void PLATE_UNIT_DrawLastMessage( PMS_SELECT_MAIN_WORK* wk, u8 view_pos_id, u8 data_id, BOOL is_select );
static void PLATE_UNIT_DrawNormal( PMS_SELECT_MAIN_WORK* wk, u8 view_pos_id, u8 data_id, BOOL is_select );

static int TOUCH_GetListPos( void );


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
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );
	
	//�����擾
	param	= pwk;

	//�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_UI_DEBUG, PMS_SELECT_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(PMS_SELECT_MAIN_WORK), HEAPID_UI_DEBUG );
  GFL_STD_MemClear( wk, sizeof(PMS_SELECT_MAIN_WORK) );

  // ������
  wk->heapID    = HEAPID_UI_DEBUG;
  wk->save_ctrl = param->save_ctrl;
  wk->pmsw_save = SaveData_GetPMSW( param->save_ctrl );
	
	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	//���b�Z�[�W
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_pms_input_dat, wk->heapID );

	//PRINT_QUE�쐬
	wk->print_que		= PRINTSYS_QUE_Create( wk->heapID );

  // �O���t�B�b�N���[�h
  PMSSelect_GRAPHIC_Load( wk );
  
  // ���񔻒�
  {
    PMS_DATA* data;
    int first_scene;

    data = PMSW_GetDataEntry( wk->pmsw_save, 0 );

    if( PMSDAT_IsComplete( data, wk->heapID ) == FALSE )
    {
      first_scene = PMSS_SCENE_ID_CALL_EDIT; // �����Ȃ�ҏW��ʂ�

      // �^���Âɂ��Ă���
      GX_SetMasterBrightness(16);
      GXS_SetMasterBrightness(16);

      HOSAKA_Printf("����Ȃ̂Œ��ڕҏW��ʂ�\n");
    }
    else
    {
      first_scene = PMSS_SCENE_ID_PLATE_SELECT; 

      //@TODO	�t�F�[�h�V�[�P���X���Ȃ��̂�
      GX_SetMasterBrightness(0);
      GXS_SetMasterBrightness(0);
    }
  
    // SCENE
	  wk->cntScene = UI_SCENE_CNT_Create( wk->heapID, c_scene_func_tbl, PMSS_SCENE_ID_MAX, first_scene, wk );
  }
  

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
  
  if( wk->bProcChange )
  {
	  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
    wk->bProcChange = FALSE;
    HOSAKA_Printf("return! \n");
  }
	
  // SCENE
  if( UI_SCENE_CNT_Main( wk->cntScene ) )
  {
    return GFL_PROC_RES_FINISH;
  }
    
  // PROC
  if( wk->bProcChange )
  {
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );
    HOSAKA_Printf("call! \n");
    return GFL_PROC_RES_CONTINUE;
  }

	// PRINT_QUE
	PRINTSYS_QUE_Main( wk->print_que );
  
  // PLATE�]��
  PLATE_CNT_Trans( wk );

#ifdef PMS_SELECT_PMSDRAW
  PMSSelect_PMSDRAW_Proc( wk );
#endif //PMS_SELECT_PMSDRAW

	// 2D�`��
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
	
  //BG���\�[�X�j��
	PMSSelect_BG_UnLoadResource( &wk->wk_bg );

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
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_pmsi_pms2_bg_main_NCLR, PALTYPE_MAIN_BG, 0x20*PLTID_BG_COMMON_M, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_pmsi_pms2_bg_main_NCLR, PALTYPE_SUB_BG,  0x20*PLTID_BG_BACK_S, 0, heapID );
	
  // ��b�t�H���g�p���b�g�]��
//	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0x20*PLTID_BG_MSG_M, 0x20, heapID );
	
  //	----- ���� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_pmsi_pms2_bg_back_NCGR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_pmsi_pms2_bg_back_NSCR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );

	//	----- ����� -----

  // �w�i
	wk->m_back_chr_pos = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(	handle, NARC_pmsi_pms2_bg_back_NCGR, BG_FRAME_BACK_M, 
            0, 0, heapID );

	GF_ASSERT( wk->m_back_chr_pos != GFL_ARCUTIL_TRANSINFO_FAIL );

	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_pmsi_pms2_bg_back01_NSCR, BG_FRAME_BACK_M,
      GFL_ARCUTIL_TRANSINFO_GetPos( wk->m_back_chr_pos ), 0, 0, heapID );
	
  // �v���[�g
  GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_pmsi_pms2_bg_main_NCGR,
						BG_FRAME_TEXT_M, 0, 0, 0, heapID );

  wk->plateScrnWork = GFL_ARCHDL_UTIL_LoadScreen( handle, NARC_pmsi_pms2_bg_main1_NSCR, FALSE,
                      &wk->platescrnData, heapID );

	GFL_ARC_CloseDataHandle( handle );

  // ���j���[��BG�ɂ��o�[�����[�h���Ă���
  

}

//-----------------------------------------------------------------------------
/**
 *	@brief  BG�J��
 *
 *	@param	PMS_SELECT_BG_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_BG_UnLoadResource( PMS_SELECT_BG_WORK* wk )
{
  // �w��BG�J��
	GFL_BG_FreeCharacterArea( BG_FRAME_BACK_M,
			GFL_ARCUTIL_TRANSINFO_GetPos(wk->m_back_chr_pos),
			GFL_ARCUTIL_TRANSINFO_GetSize(wk->m_back_chr_pos));

  GFL_HEAP_FreeMemory( wk->plateScrnWork );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  BG �v���[�g�]��
 *
 *	@param	PMS_SELECT_BG_WORK* wk 
 *	@param	view_pos_id
 *	@param	sentence_type
 *	@param	is_select 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_BG_PlateTrans( PMS_SELECT_BG_WORK* wk, u8 view_pos_id, u32 sentence_type, BOOL is_select )
{
  int i;
  int sx = wk->platescrnData->screenWidth/8;
  int sy = wk->platescrnData->screenHeight/8;

  u16* buff = (u16*)&wk->platescrnData->rawData;

  // �p���b�g�w��
  for( i=0; i<(sx*sy); i++ )
  {
    // �X�N���[���f�[�^�̏��4bit�Ńp���b�g�w��
    buff[i] = ( buff[i] & 0xFFF ) + 0x1000 * ( sentence_type + PLATE_BG_PLTT_OFS_BASE );
  }

  //@TODO �g�����]������Ηǂ��B
  
  GFL_BG_WriteScreenExpand( BG_FRAME_TEXT_M, 
    0, view_pos_id * PLATE_BG_SY + 1,
    sx, PLATE_BG_SY,
    &wk->platescrnData->rawData,
    0,  is_select * PLATE_BG_SY,
    sx, sy );

  GFL_BG_LoadScreenV_Req( BG_FRAME_TEXT_M );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�̉��g��BG�ŏ㕔�ɓ]�����N�G�X�g
 *
 *	@param	PMS_SELECT_BG_WORK* wk
 *	@param	sentence_type 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PMSSelect_BG_PlateFutterTrans( PMS_SELECT_BG_WORK* wk, u32 sentence_type )
{
  int i;
  int sx = wk->platescrnData->screenWidth/8;
  int sy = wk->platescrnData->screenHeight/8;

  u16* buff = (u16*)&wk->platescrnData->rawData;

  //@TODO �X�N���[���o�b�t�@�S�̂�u��������K�v�͂Ȃ�

  // �p���b�g�w��
  for( i=0; i<(sx*sy); i++ )
  {
    // �X�N���[���f�[�^�̏��4bit�Ńp���b�g�w��
    buff[i] = ( buff[i] & 0xFFF ) + 0x1000 * ( sentence_type + PLATE_BG_PLTT_OFS_BASE );
  }
  
  GFL_BG_WriteScreenExpand( BG_FRAME_TEXT_M, 
    0, 0,
    sx, 1,
    &wk->platescrnData->rawData,
    0,  PLATE_BG_SY-1,
    sx, sy );

  GFL_BG_LoadScreenV_Req( BG_FRAME_TEXT_M );
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
  int i;
	GFL_CLUNIT	*clunit;
  
  clunit = PMS_SELECT_GRAPHIC_GetClunit( wk->graphic );

  wk->pms_draw  = PMS_DRAW_Init( clunit, CLSYS_DRAW_MAIN, wk->print_que, wk->font, 
      PLTID_OBJ_PMS_DRAW, PMS_SELECT_PMSDRAW_NUM ,wk->heapID );

  HOSAKA_Printf("list_head_id=%d select_id=%d \n", wk->list_head_id, wk->select_id );
  
  // ���X�g�̐��𑪒�
  wk->list_max = 0;
  for( i=0; i<PMS_DATA_ENTRY_MAX; i++ )
  {
    PMS_DATA* pms;
    
    pms = PMSW_GetDataEntry( wk->pmsw_save, i );
      
    wk->list_max++; // �V�K���ڂ��܂߂�̂ł��̃^�C�~���O

    if( PMSDAT_IsComplete( pms, wk->heapID ) == FALSE )
    {
      HOSAKA_Printf( "list_max=%d \n", wk->list_max );
      break;
    }
  }

  // PMS�\���pBMPWIN����
  for( i=0; i<PMS_SELECT_PMSDRAW_NUM; i++ )
  {
    wk->pms_win[i] = GFL_BMPWIN_Create(
        BG_FRAME_TEXT_M,					// �a�f�t���[��
        2, 2 + 6 * i,			      	// �\�����W(�L�����P��)
        25, 4,    							  // �\���T�C�Y
        PLTID_BG_MSG_M,					// �p���b�g
        GFL_BMP_CHRAREA_GET_B );	// �L�����擾����
  }

  // �ȈՉ�b�`�� ������
  PLATE_CNT_Setup( wk );
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
  PMS_DRAW_Main( wk->pms_draw );
}

#endif // PMS_SELECT_PMSDRAW

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�Ǘ� �S�v���[�g���j�b�g��������
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PLATE_CNT_Setup( PMS_SELECT_MAIN_WORK* wk )
{
  PLATE_CNT_UpdateCore( wk, FALSE );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�Ǘ� �S�v���[�g���j�b�g���X�V
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void PLATE_CNT_UpdateAll( PMS_SELECT_MAIN_WORK* wk )
{
  PLATE_CNT_UpdateCore( wk, TRUE );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�Ǘ� �S�v���[�g���j�b�g���X�V �R�A
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PLATE_CNT_UpdateCore( PMS_SELECT_MAIN_WORK* wk, BOOL is_check_print_end )
{ 
  int i;
    
  // �܂������]�����I�����Ă��Ȃ����̂�����Ƃ��͍X�V���Ȃ�
  if( is_check_print_end && PMS_DRAW_IsPrintEnd( wk->pms_draw ) == FALSE )
  {
    return;
  }

  // ��[�X�N���[�����N���A
  GFL_BG_ClearScreenCodeVReq( BG_FRAME_TEXT_M, 0 );

  // ���X�g���擪�łȂ���΍ŏ㕔�Ƀt�b�^��\��
  if( wk->list_head_id > 0 )
  {
    PMS_DATA* pms;
    u8 futter_id;
    u32 sentence_type;
    
    futter_id = wk->list_head_id - 1;
    pms = PMSW_GetDataEntry( wk->pmsw_save, futter_id );
    sentence_type = PMSDAT_GetSentenceType( pms );

    PMSSelect_BG_PlateFutterTrans( &wk->wk_bg, sentence_type );
  }

  for( i=0; (wk->list_head_id+i) < wk->list_max && i < PMS_SELECT_PMSDRAW_NUM; i++ )
  {
    u8 data_id = wk->list_head_id + i;
    BOOL is_select = ( data_id == wk->select_id );
    
    if( data_id == wk->list_max-1 )
    {
      // �u�����炵���@���b�Z�[�W���@��������v
      PLATE_UNIT_DrawLastMessage( wk, i, NULL, is_select );
    }
    else
    {
      // �ȈՉ�b�`��
      PLATE_UNIT_DrawNormal( wk, i, data_id, is_select );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g���� �又��
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PLATE_CNT_Main( PMS_SELECT_MAIN_WORK* wk )
{
  // �L�[����
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP )
  {
    if( wk->select_id > 0 )
    {
      wk->select_id--;

      // ���X�g�̐擪��I�����Ă����ꍇ�̓��X�g���ƈړ�
      if( wk->select_id+1 == wk->list_head_id )
      {
        wk->list_head_id--;

        GF_ASSERT( wk->list_head_id >= 0 );
      }

      //@TODO �d���悤�Ȃ�A
      //1:�y�[�W�S�̂��X�V����Ȃ�����UpdateALl���X�N���[���̃J���[��u�����鏈���ɂ���
      //2:�ړ��̎��̓R�s�[���g���B
      PLATE_CNT_UpdateAll( wk );

      GFL_SOUND_PlaySE( SE_MOVE_CURSOR );
      
      HOSAKA_Printf("list_head_id=%d select_id=%d \n", wk->list_head_id, wk->select_id );
    }
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN )
  {
    if( wk->select_id < wk->list_max-1 )
    {
      wk->select_id++;

      // ���X�g�̖�����I�����Ă����ꍇ�̓��X�g���ƈړ�
      if( ( wk->select_id == wk->list_head_id + PMS_SELECT_PMSDRAW_NUM - 1 ) )
      {
        wk->list_head_id++;

        GF_ASSERT( wk->list_head_id >= 0 );
      }
        
      PLATE_CNT_UpdateAll( wk );

      GFL_SOUND_PlaySE( SE_MOVE_CURSOR );
     
      HOSAKA_Printf("list_head_id=%d select_id=%d \n", wk->list_head_id, wk->select_id );
    }
  }

  // �^�b�`����
  {
    int list_pos = TOUCH_GetListPos();

    if( list_pos != -1 )
    {
      wk->select_id =  wk->list_head_id + list_pos;

      HOSAKA_Printf("list_pos = %d, select_id = %d \n", list_pos, wk->select_id );
      
      PLATE_CNT_UpdateAll( wk );
    }
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief  �S�v���[�g��]��
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PLATE_CNT_Trans( PMS_SELECT_MAIN_WORK* wk )
{
  int i;

  for( i=0; i < PMS_SELECT_PMSDRAW_NUM; i++ )
  {
    PLATE_UNIT_Trans( wk->print_que, wk->pms_win[ i ], &wk->pms_win_tranReq[ i ] );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�]��
 *
 *	@param	PRINT_QUE* print_que
 *	@param	win
 *	@param	transReq 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL PLATE_UNIT_Trans( PRINT_QUE* print_que, GFL_BMPWIN* win, BOOL *transReq )
{ 
  GFL_BMP_DATA* bmp = GFL_BMPWIN_GetBmp( win );

  if( *transReq == TRUE && !PRINTSYS_QUE_IsExistTarget( print_que, bmp ) )
  {
    GFL_BMPWIN_MakeTransWindow( win );
//    GFL_BMPWIN_TransVramCharacter( win );
    *transReq = FALSE;
    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �����炵���@���b�Z�[�W���@��������
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk
 *	@param	view_pos_id
 *	@param	data_id
 *	@param	is_select 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PLATE_UNIT_DrawLastMessage( PMS_SELECT_MAIN_WORK* wk, u8 view_pos_id, u8 data_id, BOOL is_select )
{
  PRINTSYS_LSB color;
  STRBUF* buf;

  GF_ASSERT( view_pos_id < PMS_SELECT_PMSDRAW_NUM );
  
  // �ȈՉ�b��\�����Ȃ�Ώ���
  if( PMS_DRAW_IsPrinting( wk->pms_draw, view_pos_id ) )
  {
    PMS_DRAW_Clear( wk->pms_draw, view_pos_id, FALSE );
  }

  buf = GFL_MSG_CreateString( wk->msg, pms_input01_01 );
  
  // �v���[�g�`��
  PMSSelect_BG_PlateTrans( &wk->wk_bg, view_pos_id, 0, is_select );
  
  // �I����Ԃɂ���ĕ����`��G���A�̃J���[���w��
  if( is_select )
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->pms_win[ view_pos_id ] ), 0x2 );
    color = PRINTSYS_LSB_Make( 0xe, 0xf, 0x2 );
  }
  else
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->pms_win[ view_pos_id ] ), 0x1 );
    color = PRINTSYS_LSB_Make( 0xe, 0xf, 0x1 );
  }

  PRINTSYS_PrintQueColor( wk->print_que, GFL_BMPWIN_GetBmp(wk->pms_win[ view_pos_id ]), 0, 0, buf, wk->font, color );

  GFL_STR_DeleteBuffer( buf );

  wk->pms_win_tranReq[ view_pos_id ] = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �v���[�g�`��
 *
 *	@param	PMS_SELECT_MAIN_WORK* wk
 *	@param	view_pos_id
 *	@param	data_id 
 *
 *	@retval 
 */
//-----------------------------------------------------------------------------
static void PLATE_UNIT_DrawNormal( PMS_SELECT_MAIN_WORK* wk, u8 view_pos_id, u8 data_id, BOOL is_select )
{
  PMS_DATA* pms;
  
  GF_ASSERT( view_pos_id < PMS_SELECT_PMSDRAW_NUM );

  // �\�����Ȃ�Ώ���
  if( PMS_DRAW_IsPrinting( wk->pms_draw, view_pos_id ) )
  {
    PMS_DRAW_Clear( wk->pms_draw, view_pos_id, FALSE );
  }
     
  pms = PMSW_GetDataEntry( wk->pmsw_save, data_id );

  // �p���b�g�ɂ���ăv���[�g�̔w�i�F��ύX
  {
    u32 sentence_type = PMSDAT_GetSentenceType( pms );
    PMSSelect_BG_PlateTrans( &wk->wk_bg, view_pos_id, sentence_type, is_select );
  }

  // �I����Ԃɂ���ĕ����`��G���A�̐F�w��
  if( is_select )
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->pms_win[ view_pos_id ] ), 0x2 );
    PMS_DRAW_SetPrintColor( wk->pms_draw, PRINTSYS_LSB_Make( 0xe, 0xf, 0x2 ) );
  }
  else
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->pms_win[ view_pos_id ] ), 0x1 );
    PMS_DRAW_SetPrintColor( wk->pms_draw, PRINTSYS_LSB_Make( 0xe, 0xf, 0x1 ) );
  }

  //  �ȈՉ�b�\��
  if( PMSDAT_IsComplete( pms, wk->heapID ) == TRUE )
  {
    PMS_DRAW_Print( wk->pms_draw, wk->pms_win[ view_pos_id ], pms, view_pos_id );
    wk->pms_win_tranReq[ view_pos_id ] = TRUE;
  }
  
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�͈̔͂��^�b�`���Ă��邩�H
 *
 *	@param	void
 *
 *	@retval TRUE : ���Ă���
 */
//-----------------------------------------------------------------------------
static int TOUCH_GetListPos( void )
{
  u32 px, py;

  if( GFL_UI_TP_GetPointTrg( &px, &py ) )
  {
    if( px < GX_LCD_SIZE_X - (8*3) && py > PLATE_BG_START_PY * 8 )
    {
        // �L�����ɕϊ�
        py /= 8;
        // �擪�܂ł��炵
        py -= PLATE_BG_START_PY;
        // ���X�g�̏ꏊ���擾
        py /= PLATE_BG_SY;

        // �^�b�`�\�Ȃ̂�3����
        if( py < PMS_SELECT_PMSDRAW_NUM-1 )
        {
          return py;
        }
    }
  }

  return -1;
}

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
  TOUCHBAR_ICON result;

	//�^�b�`�o�[���C������
	PMSSelect_TOUCHBAR_Main( wk->touchbar );

  // �^�b�`�o�[���͏�Ԏ擾
  result = TOUCHBAR_GetTrg( wk->touchbar );

  // �^�b�`�o�[���͔���
  switch( result )
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

  // �v���[�g����
  PLATE_CNT_Main( wk );
    
  // ���j���[���J��
  if( ( TOUCH_GetListPos() != -1 ) || (GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ) )
  {
    PMS_DATA* pms;

    GFL_SOUND_PlaySE( SE_DECIDE );
    
    pms = PMSW_GetDataEntry( wk->pmsw_save, wk->select_id );

    if( PMSDAT_IsComplete( pms, wk->heapID ) )
    {
      UI_SCENE_CNT_SetNextScene( cnt, PMSS_SCENE_ID_CMD_SELECT );
    }
    else
    {
      // �I��̈悪�V�K�Ȃ璼�ڃW�����v
      UI_SCENE_CNT_SetNextScene( cnt, PMSS_SCENE_ID_CALL_EDIT );
    }

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
 *	@brief  �v���[�g�ɑ΂���R�}���h�I�� ������
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
 *	@brief  �v���[�g�ɑ΂���R�}���h�I�� �又��
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
 *	@brief  �v���[�g�ɑ΂���R�}���h�I�� �㏈��
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
  u8 seq = UI_SCENE_CNT_GetSubSeq( cnt );

  switch( seq )
  {
  case 0:
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0);
    UI_SCENE_CNT_IncSubSeq( cnt );
    break;
  case 1:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      PMSSelect_GRAPHIC_UnLoad( wk );

#if PM_DEBUG
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
      {
        PMS_DATA data;
        PMSDAT_Init( &data, 0 );
        pmsi = PMSI_PARAM_Create(PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, &data, TRUE, wk->save_ctrl, wk->heapID );
      }
      else
#endif
      {
        pmsi = PMSI_PARAM_Create(PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, NULL, TRUE, wk->save_ctrl, wk->heapID );
      }

      if( pmsi )

      // PROC�ؑ� ���͉�ʌĂяo��
      GFL_PROC_SysCallProc( NO_OVERLAY_ID, &ProcData_PMSInput, pmsi );
      wk->subproc_work = pmsi;

      // PROC�ؑփt���OON
      wk->bProcChange = TRUE;
      return TRUE;
    }
    break;

  default : GF_ASSERT(0);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ҏW��ʏI��
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

    if( PMSI_PARAM_CheckModified( wk->subproc_work ) )
    {
      PMS_DATA pms;

      // �ҏW�������b�Z�[�W���Z�[�u
      PMSI_PARAM_GetInputDataSentence( wk->subproc_work, &pms );

      PMSW_SetDataEntry( wk->pmsw_save, wk->select_id, &pms );
    }

    // �p�����[�^�폜
    PMSI_PARAM_Delete( wk->subproc_work );

    // �O���t�B�b�N�����[�h
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


