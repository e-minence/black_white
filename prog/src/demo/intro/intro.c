//=============================================================================
/**
 *
 *	@file		intro.c
 *	@brief  3D�f���Đ��A�v��
 *	@author	genya hosaka
 *	@data		2009.12.10
 *
 */
//=============================================================================
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/brightness.h"

//�^�X�N���j���[
#include "app/app_taskmenu.h"

//�A�v�����ʑf��
#include "app/app_menu_common.h"

// ������֘A
#include "font/font.naix"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h" //PRINT_QUE

//�Ȉ�CLWK�ǂݍ��݁��J�����[�e�B���e�B�[
#include "ui/ui_easy_clwk.h"

//�^�b�`�o�[
#include "ui/touchbar.h"

//���[�J���w�b�_
#include "intro_sys.h"

//�`��ݒ�
#include "intro_graphic.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "sound/wb_sound_data.sadl"


//�O�����J
#include "demo/intro.h"


#include "message.naix"

#include "intro_mcss.h" // for INTRO_MCSS_WORK
#include "intro_g3d.h" // for INTRO_G3D_WORK
#include "intro_particle.h" // for INTRO_PARTICLE_WORK

#include "intro_cmd.h"

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================

FS_EXTERN_OVERLAY(ui_common);

#define CHECK_KEY_TRG( key ) ( ( GFL_UI_KEY_GetTrg() & (key) ) == (key) )

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================


//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heapID;

  INTRO_PARAM*        param;

	//�`��ݒ�
	INTRO_GRAPHIC_WORK	*graphic;

	//�t�H���g
	GFL_FONT									*font;

	//�v�����g�L���[
//	PRINT_QUE									*print_que;
	GFL_MSGDATA								*msg;

  INTRO_CMD_WORK*   cmd;
  INTRO_MCSS_WORK*  mcss;
  INTRO_G3D_WORK*   g3d;
  INTRO_PARTICLE_WORK*  ptc;

} INTRO_MAIN_WORK;


//=============================================================================
/**
 *							�f�[�^��`
 */
//=============================================================================
//-------------------------------------
///	�O���Ńv���Z�b�g�o�^���邽��
//=====================================
const u32 Intro_Se_PresetData[] =
{
  SEQ_SE_NAGERU,
  SEQ_SE_BOWA2,
	SEQ_SE_OPEN2,
	SEQ_SE_KON,

	SEQ_BGM_STARTING,
	SEQ_BGM_STARTING2,
};

const u32 Intro_Se_PresetNum  = 6;

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
static GFL_PROC_RESULT IntroProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT IntroProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT IntroProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );

//-------------------------------------
///	�ėp�������[�e�B���e�B
//=====================================

//=============================================================================
/**
 *								�O�����J
 */
//=============================================================================
const GFL_PROC_DATA ProcData_Intro = 
{
	IntroProc_Init,
	IntroProc_Main,
	IntroProc_Exit,
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
 *	@param	*pwk					INTRO_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IntroProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
	INTRO_MAIN_WORK * wk;
	INTRO_PARAM * param;

	//�I�[�o�[���C�ǂݍ���
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );
	
	//�����擾
	param	= pwk;

	//�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_INTRO, INTRO_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(INTRO_MAIN_WORK), HEAPID_INTRO );
  GFL_STD_MemClear( wk, sizeof(INTRO_MAIN_WORK) );

  // ������
  wk->heapID      = HEAPID_INTRO;
  wk->param       = param;
	
	//�`��ݒ菉����
	wk->graphic	= INTRO_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, wk->param->scene_id, wk->heapID );

	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	//���b�Z�[�W
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
			NARC_message_mictest_dat, wk->heapID );

	//PRINT_QUE�쐬
//	wk->print_que		= PRINTSYS_QUE_Create( wk->heapID );

  // MCSS������
  wk->mcss = INTRO_MCSS_Create( wk->heapID, wk->param->scene_id );

  // 3D�֘A������
  wk->g3d = INTRO_G3D_Create( wk->graphic, wk->param->scene_id, wk->heapID );
  wk->ptc = INTRO_PARTICLE_Create( wk->graphic, wk->heapID );

  // �R�}���h������
  wk->cmd = Intro_CMD_Init( wk->g3d, wk->ptc, wk->mcss, wk->param, wk->graphic, wk->heapID );

  // �u���C�h�l�X�ݒ� �^����
//  SetBrightness( -16, (PLANEMASK_BG0|PLANEMASK_BG2|PLANEMASK_BG3|PLANEMASK_OBJ), MASK_MAIN_DISPLAY );
  // �t�F�[�h�C�� ���N�G�X�g
//  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 2 );

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  PROC �I������
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					INTRO_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IntroProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	INTRO_MAIN_WORK* wk = mywk;
/*  
  if( *seq == 0 )
  {
    // �t�F�[�h�A�E�g ���N�G�X�g
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 2 );
    (*seq)++;
    return GFL_PROC_RES_CONTINUE;
  }
  else if( GFL_FADE_CheckFade() == TRUE )
  {
    // �t�F�[�h���͏����ɓ���Ȃ�
    return GFL_PROC_RES_CONTINUE;
  }
*/

  INTRO_PARTICLE_Exit( wk->ptc );
  INTRO_MCSS_Exit( wk->mcss );
  INTRO_G3D_Exit( wk->g3d );

  HOSAKA_Printf("%d, %p \n", wk->msg, wk->msg );

	//���b�Z�[�W�j��
	GFL_MSG_Delete( wk->msg );

	//PRINT_QUE
//	PRINTSYS_QUE_Delete( wk->print_que );

	//FONT
	GFL_FONT_Delete( wk->font );
  
  //�R�}���h �j��
  Intro_CMD_Exit( wk->cmd );

	//�`��ݒ�j��
	INTRO_GRAPHIC_Exit( wk->graphic );

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
 *	@param	*pwk					INTRO_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IntroProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	INTRO_MAIN_WORK* wk = mywk;
  BOOL is_coutinue;

#if 0
  // �t�F�[�h���͏������Ȃ�
  if( GFL_FADE_CheckFade() == TRUE )
  {
    return GFL_PROC_RES_CONTINUE;
  }
  // ����L�[�ŃA�v���I��
  if( CHECK_KEY_TRG( PAD_BUTTON_DEBUG ) )
  {
    return GFL_PROC_RES_FINISH;
  }
#endif

	//�R�}���h���s
  is_coutinue = Intro_CMD_Main( wk->cmd );
  
  // �I������
  if( is_coutinue == FALSE )
  {
    return GFL_PROC_RES_FINISH;
  }

	//PRINT_QUE
//	PRINTSYS_QUE_Main( wk->print_que );

	//2D�`��
	INTRO_GRAPHIC_2D_Draw( wk->graphic );

  //3D�`��
  INTRO_GRAPHIC_3D_StartDraw( wk->graphic );

	// �|�P�����A�j���Ď�
	INTRO_MCSS_PokeAnime( wk->mcss );

  INTRO_MCSS_Main( wk->mcss );
  INTRO_PARTICLE_Main( wk->ptc );
  INTRO_G3D_Main( wk->g3d );

  INTRO_GRAPHIC_3D_EndDraw( wk->graphic );

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================


