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
#include "system/wipe.h"
#include "system/brightness.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"

#include "arc_def.h"

//�`��ݒ�
#include "hoge_graphic.h"


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

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  UI_TEMPLATE_HEAP_SIZE = 0x30000,  ///< �q�[�v�T�C�Y
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


	//�`��ݒ�j��
	HOGE_GRAPHIC_Exit( wk->p_graphic );


  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( wk->heap_id );

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

	//�`��ݒ胁�C��
	HOGE_GRAPHIC_2D_Draw( wk->p_graphic );

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

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARC_UTIL_TransVramPalette( ARCID_MICTEST_GRA, NARC_mictest_back_bg_down_NCLR, PALTYPE_MAIN_BG, 0, 0x20, heap_id );
	GFL_ARC_UTIL_TransVramPalette( ARCID_MICTEST_GRA, NARC_mictest_back_bg_up_NCLR, PALTYPE_SUB_BG, 0, 0x20, heap_id );
	
  //	----- ����� -----
	GFL_ARC_UTIL_TransVramBgCharacter(	ARCID_MICTEST_GRA, NARC_mictest_back_bg_down_NCGR,
						GFL_BG_FRAME1_S, 0, 0, 0, heap_id );
	GFL_ARC_UTIL_TransVramScreen(	ARCID_MICTEST_GRA, NARC_mictest_back_bg_down_NSCR,
						GFL_BG_FRAME1_S, 0, 0, 0, heap_id );	

	//	----- ���� -----
	GFL_ARC_UTIL_TransVramBgCharacter(	ARCID_MICTEST_GRA, NARC_mictest_back_bg_up_NCGR,
						GFL_BG_FRAME1_M, 0, 0, 0, heap_id );
	GFL_ARC_UTIL_TransVramScreen(	ARCID_MICTEST_GRA, NARC_mictest_back_bg_up_NSCR,
						GFL_BG_FRAME1_M, 0, 0, 0, heap_id );		
}

