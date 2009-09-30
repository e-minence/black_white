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
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "font/font.naix"

#include "arc_def.h"

//INFOWIN
#include "infowin/infowin.h"

//�`��ݒ�
#include "hoge_graphic.h"

//�^�b�`�o�[
#include "ui/touchbar.h"

//�O�����J
#include "ui_template.h"

#include "message.naix" // GMM

//@TODO �Ƃ肠�����}�C�N�e�X�g�̃��\�[�X
#include "mictest.naix"	// �A�[�J�C�u
#include "msg/msg_mictest.h"  // GMM

// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜����
#define UI_POKE_ICON

// �v���Z�X��`�f�[�^
const GFL_PROC_DATA UITemplateProcData = {
	UITemplateProc_Init,
	UITemplateProc_Main,
	UITemplateProc_Exit,
};

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
	BG_FRAME_BAR_M	= GFL_BG_FRAME0_M,
	BG_FRAME_BACK_M	= GFL_BG_FRAME1_M,
	BG_FRAME_BACK_S	= GFL_BG_FRAME1_S,
};
//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	//���C��BG
	PLTID_BG_BACK_M			= 0,
	PLTID_BG_TOUCHBAR_M	= 13,
	PLTID_BG_INFOWIN_M	= 15,
	//�T�uBG
	PLTID_BG_BACK_S	=		 0,

	//���C��OBJ
	PLTID_OBJ_TOUCHBAR_M	= 0,
};

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//@TODO �v�����gQUE�Ή�
//--------------------------------------------------------------
///	BG�Ǘ����[�N
//==============================================================
typedef struct {
  GFL_FONT* fontHandle;
} UI_TEMPLATE_BG_WORK;

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct {
  HEAPID heap_id;

	UI_TEMPLATE_BG_WORK   wk_bg;

	//�`��ݒ�
	HOGE_GRAPHIC_WORK			*p_graphic;

	//�^�b�`�o�[
	TOUCHBAR_WORK					*p_touchbar;
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
static void UITemplate_BG_LoadResource( UI_TEMPLATE_BG_WORK* wk, HEAPID heap_id );

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC ����������
 *
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	*pwk
 *	@param	*mywk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT UITemplateProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
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
	wk->p_graphic	= HOGE_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heap_id );

	//BG���\�[�X�ǂݍ���
	UITemplate_BG_LoadResource( &wk->wk_bg, wk->heap_id );

	//INFOWIN�̏�����
	{
		GAME_COMM_SYS_PTR comm;
		comm	= GAMESYSTEM_GetGameCommSysPtr(param->p_gamesys);
		INFOWIN_Init( BG_FRAME_BAR_M, 
				PLTID_BG_INFOWIN_M, comm, wk->heap_id );
	}

	//�^�b�`�o�[�̏�����
	{	
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
		TOUCHBAR_SETUP	touchbar_setup;
		touchbar_setup.p_item		= touchbar_icon_tbl;
		touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);
		touchbar_setup.p_unit		= HOGE_GRAPHIC_GetClunit( wk->p_graphic );
		touchbar_setup.bar_frm	= BG_FRAME_BAR_M;
		touchbar_setup.bg_plt		= PLTID_BG_TOUCHBAR_M;
		touchbar_setup.obj_plt	= PLTID_OBJ_TOUCHBAR_M;
		touchbar_setup.mapping	= APP_COMMON_MAPPING_128K;

		wk->p_touchbar	= TOUCHBAR_Init( &touchbar_setup, wk->heap_id );
	}


	//@todo	�t�F�[�h�V�[�P���X���Ȃ��̂�
	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(0);

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  PROC �I������
 *
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	*pwk
 *	@param	*mywk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT UITemplateProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	UI_TEMPLATE_MAIN_WORK* wk = mywk;

	//�^�b�`�o�[�j��
	TOUCHBAR_Exit( wk->p_touchbar );

	//INFOWIN�j��
	INFOWIN_Exit();

	//�`��ݒ�j��
	HOGE_GRAPHIC_Exit( wk->p_graphic );


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
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	*pwk
 *	@param	*mywk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT UITemplateProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	UI_TEMPLATE_MAIN_WORK* wk = mywk;

  // �f�o�b�O�{�^���ŃA�v���I��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG )
  {
    return GFL_PROC_RES_FINISH;
  }

	//INFOWIN���C��
	INFOWIN_Update();

	//�`��ݒ胁�C��
	HOGE_GRAPHIC_2D_Draw( wk->p_graphic );

	//TOUCHBAR
	TOUCHBAR_Main( wk->p_touchbar );

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

