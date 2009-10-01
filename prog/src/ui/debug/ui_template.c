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
#include "ui_template_graphic.h"

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
	UI_TEMPLATE_GRAPHIC_WORK			*p_graphic;

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

//-------------------------------------
///	INFOWIN
//=====================================
static void UITemplate_INFOWIN_Init( GAMESYS_WORK *p_gamesys, HEAPID heap_id );
static void UITemplate_INFOWIN_Exit( void );
static void UITemplate_INFOWIN_Main( void );
//-------------------------------------
///	�^�b�`�o�[
//=====================================
static TOUCHBAR_WORK * UITemplate_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heap_id );
static void UITemplate_TOUCHBAR_Exit( TOUCHBAR_WORK	*p_touchbar );
static void UITemplate_TOUCHBAR_Main( TOUCHBAR_WORK	*p_touchbar );

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
	wk->p_graphic	= UI_TEMPLATE_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heap_id );

	//BG���\�[�X�ǂݍ���
	UITemplate_BG_LoadResource( &wk->wk_bg, wk->heap_id );

	//INFOWIN�̏�����
	UITemplate_INFOWIN_Init( param->p_gamesys, wk->heap_id );

	//�^�b�`�o�[�̏�����
	{	
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->p_graphic );
		wk->p_touchbar	= UITemplate_TOUCHBAR_Init( clunit, wk->heap_id );
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


	//�^�b�`�o�[
	UITemplate_TOUCHBAR_Exit( wk->p_touchbar );

	//INFWIN
	UITemplate_INFOWIN_Exit();

	//�`��ݒ�j��
	UI_TEMPLATE_GRAPHIC_Exit( wk->p_graphic );


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

	//�^�b�`�o�[���C������
	UITemplate_TOUCHBAR_Main( wk->p_touchbar );

	//INFWIN
	UITemplate_INFOWIN_Main();

	//�`��ݒ胁�C��
	UI_TEMPLATE_GRAPHIC_2D_Draw( wk->p_graphic );



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
//=============================================================================
/**
 *		INFOWIN
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	INFOWIN������
 *
 *	@param	GAMESYS_WORK *p_gamesys	�Q�[���V�X�e��
 *	@param	heap_id		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void UITemplate_INFOWIN_Init( GAMESYS_WORK *p_gamesys, HEAPID heap_id )
{	
	GAME_COMM_SYS_PTR comm;
	comm	= GAMESYSTEM_GetGameCommSysPtr(p_gamesys);
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
	touchbar_setup.p_unit		= clunit;
	touchbar_setup.bar_frm	= BG_FRAME_BAR_M;
	touchbar_setup.bg_plt		= PLTID_BG_TOUCHBAR_M;
	touchbar_setup.obj_plt	= PLTID_OBJ_TOUCHBAR_M;
	touchbar_setup.mapping	= APP_COMMON_MAPPING_128K;

	return TOUCHBAR_Init( &touchbar_setup, heap_id );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR�j��
 *
 *	@param	TOUCHBAR_WORK	*p_touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void UITemplate_TOUCHBAR_Exit( TOUCHBAR_WORK	*p_touchbar )
{	
	TOUCHBAR_Exit( p_touchbar );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR���C������
 *
 *	@param	TOUCHBAR_WORK	*p_touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void UITemplate_TOUCHBAR_Main( TOUCHBAR_WORK	*p_touchbar )
{	
	TOUCHBAR_Main( p_touchbar );
}
