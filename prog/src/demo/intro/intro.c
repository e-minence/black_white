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

//INFOWIN
#include "infowin/infowin.h"

//���[�J���w�b�_
#include "intro_sys.h"

//�`��ݒ�
#include "intro_graphic.h"

//�A�[�J�C�u
#include "arc_def.h"

//�O�����J
#include "demo/intro.h"

#include "message.naix"

#include "intro_mcss.h"
#include "intro_cmd.h"

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================
//#define INTRO_INFOWIN
//#define INTRO_TOUCHBAR
//#define INTRO_TASKMENU

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

#ifdef INTRO_TOUCHBAR
	//�^�b�`�o�[
	TOUCHBAR_WORK							*touchbar;
	//�ȉ��J�X�^���{�^���g�p����ꍇ�̃T���v�����\�[�X
	u32												ncg_btn;
	u32												ncl_btn;
	u32												nce_btn;
#endif //INTRO_TOUCHBAR

	//�t�H���g
	GFL_FONT									*font;

	//�v�����g�L���[
	PRINT_QUE									*print_que;
	GFL_MSGDATA								*msg;

#ifdef INTRO_TASKMENU
	//�^�X�N���j���[
	APP_TASKMENU_RES					*menu_res;
	APP_TASKMENU_WORK					*menu;
#endif //INTRO_TASKMENU

#ifdef	INTRO_PRINT_TOOL
	//�v�����g���[�e�B���e�B
	PRINT_UTIL								print_util;
	u32												seq;
#endif	//INTRO_PRINT_TOOL

  INTRO_CMD_WORK*   cmd;
  INTRO_MCSS_WORK*  mcss;

} INTRO_MAIN_WORK;


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
static GFL_PROC_RESULT IntroProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT IntroProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT IntroProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );

//-------------------------------------
///	�ėp�������[�e�B���e�B
//=====================================

#ifdef INTRO_INFOWIN
//-------------------------------------
///	INFOWIN
//=====================================
static void Intro_INFOWIN_Init( GAMESYS_WORK *gamesys, HEAPID heapID );
static void Intro_INFOWIN_Exit( void );
static void Intro_INFOWIN_Main( void );
#endif //INTRO_INFOWIN

#ifdef INTRO_TOUCHBAR
//-------------------------------------
///	�^�b�`�o�[
//=====================================
static TOUCHBAR_WORK * Intro_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heapID );
static void Intro_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar );
static void Intro_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar );
//�ȉ��J�X�^���{�^���g�p�T���v���p
static TOUCHBAR_WORK * Intro_TOUCHBAR_InitEx( INTRO_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID );
static void Intro_TOUCHBAR_ExitEx( INTRO_MAIN_WORK *wk );
#endif //INTRO_TOUCHBAR

#ifdef INTRO_TASKMENU
//-------------------------------------
///	���X�g�V�X�e���͂��A������
//=====================================
static APP_TASKMENU_WORK * Intro_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heapID );
static void Intro_TASKMENU_Exit( APP_TASKMENU_WORK *menu );
static void Intro_TASKMENU_Main( APP_TASKMENU_WORK *menu );
#endif //INTRO_TASKMENU

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
	INTRO_MAIN_WORK *wk;
	INTRO_PARAM *param;

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
	wk->graphic	= INTRO_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, wk->heapID );

	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	//���b�Z�[�W
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
			NARC_message_mictest_dat, wk->heapID );

	//PRINT_QUE�쐬
	wk->print_que		= PRINTSYS_QUE_Create( wk->heapID );

  // MCSS������
  wk->mcss = INTRO_MCSS_Create( wk->heapID );

  // �R�}���h������
  wk->cmd = Intro_CMD_Init( wk->mcss, wk->param, wk->heapID );

#ifdef INTRO_INFOWIN
	//INFOWIN�̏�����
	Intro_INFOWIN_Init( param->gamesys, wk->heapID );
#endif //INTRO_INFOWIN

#ifdef INTRO_TOUCHBAR
	//�^�b�`�o�[�̏�����
	{	
		GFL_CLUNIT	*clunit	= INTRO_GRAPHIC_GetClunit( wk->graphic );
		wk->touchbar	= Intro_TOUCHBAR_InitEx( wk, clunit, wk->heapID );
	}
#endif //INTRO_TOUCHBAR

#ifdef INTRO_TASKMENU
	//TASKMENU���\�[�X�ǂݍ��݁�������
	wk->menu_res	= APP_TASKMENU_RES_Create( BG_FRAME_BAR_M, PLTID_BG_TASKMENU_M, wk->font, wk->print_que, wk->heapID );
	wk->menu			= Intro_TASKMENU_Init( wk->menu_res, wk->msg, wk->heapID );
#endif //INTRO_TASKMENU

  // @TODO �ėp���̂��߂ɁA�������R�}���h��p�ӂ���ׂ�
  // �u���C�h�l�X�ݒ� �^����
  SetBrightness( -16, (PLANEMASK_BG0|PLANEMASK_BG2|PLANEMASK_BG3|PLANEMASK_OBJ), MASK_MAIN_DISPLAY );

  // �t�F�[�h�C�� ���N�G�X�g
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 2 );

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
  
  INTRO_MCSS_Exit( wk->mcss );

#ifdef INTRO_TASKMENU
	//TASKMENU�V�X�e�������\�[�X�j��
	Intro_TASKMENU_Exit( wk->menu );
	APP_TASKMENU_RES_Delete( wk->menu_res );	
#endif //INTRO_TASKMENU

#ifdef INTRO_TOUCHBAR
	//�^�b�`�o�[
	Intro_TOUCHBAR_ExitEx( wk );
#endif //INTRO_TOUCHBAR

#ifdef INTRO_INFOWIN
	//INFWIN
	Intro_INFOWIN_Exit();
#endif //INTRO_INFOWIN

	//���b�Z�[�W�j��
	GFL_MSG_Delete( wk->msg );

	//PRINT_QUE
	PRINTSYS_QUE_Delete( wk->print_que );

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


static void debug_camera_test( GFL_G3D_CAMERA* camera )
{ 
  VecFx32 pos;

  static BOOL mode = 0;

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
  {
    mode = (mode+1)%3;

    if( mode == 0 )
    {
      OS_Printf("mode=%d Pos\n",mode);
    }
    else if( mode == 1 )
    {
      OS_Printf("mode=%d CamUp\n",mode);
    }
    else
    {
      OS_Printf("mode=%d Target\n",mode);
    }
  }
  
  if( mode == 0 )
  {
    GFL_G3D_CAMERA_GetPos( camera, &pos );
  }
  else if( mode == 1 )
  {
    GFL_G3D_CAMERA_GetCamUp( camera, &pos );
  }
  else
  {
    GFL_G3D_CAMERA_GetTarget( camera, &pos );
  }

  if( GFL_UI_KEY_GetCont() & PAD_KEY_UP )
  {
    pos.y += 1;
    OS_Printf("mode=%d pos{ 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }
  else if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )
  {
    pos.y -= 1;
    OS_Printf("mode=%d pos{ 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }    
  else if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
  {
    pos.x += 1;
    OS_Printf("mode=%d pos{ 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }
  else if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
  {
    pos.x -= 1;
    OS_Printf("mode=%d pos{ 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    pos.z += 1;
    OS_Printf("mode=%d pos{ 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    pos.z -= 1;
    OS_Printf("mode=%d pos{ 0x%x, 0x%x, 0x%x } \n", mode, pos.x, pos.y, pos.z );
  }
  
  // �f�[�^�Z�b�g
  if( mode == 0 )
  {
    GFL_G3D_CAMERA_SetPos( camera, &pos );
  }
  else if( mode == 1 )
  {
    GFL_G3D_CAMERA_SetCamUp( camera, &pos );
  }
  else
  {
    GFL_G3D_CAMERA_SetTarget( camera, &pos );
  }

  // �f�[�^�f���o��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    GFL_G3D_CAMERA_GetPos( camera, &pos );
    OS_Printf("pos { 0x%x, 0x%x, 0x%x } \n", pos.x, pos.y, pos.z );
    GFL_G3D_CAMERA_GetCamUp( camera, &pos );
    OS_Printf("CamUp { 0x%x, 0x%x, 0x%x } \n", pos.x, pos.y, pos.z );
    GFL_G3D_CAMERA_GetTarget( camera, &pos );
    OS_Printf("Taraget { 0x%x, 0x%x, 0x%x } \n", pos.x, pos.y, pos.z );
  }
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

  // �t�F�[�h���͏������Ȃ�
  if( GFL_FADE_CheckFade() == TRUE )
  {
    return GFL_PROC_RES_CONTINUE;
  }

  // ����L�[�ŃA�v���I��
  if( 
      CHECK_KEY_TRG( PAD_BUTTON_DEBUG ) 
    )
  {
    return GFL_PROC_RES_FINISH;
  }

#ifdef INTRO_TOUCHBAR
	//�^�b�`�o�[���C������
	Intro_TOUCHBAR_Main( wk->touchbar );
#endif //INTRO_TOUCHBAR

#ifdef INTRO_INFOWIN
	//INFWIN���C������
	Intro_INFOWIN_Main();
#endif //INTRO_INFOWIN

#ifdef INTRO_TASKMENU
	//�^�X�N���j���[���C������
	Intro_TASKMENU_Main( wk->menu );
#endif //INTRO_TASKMENU

	//PRINT_QUE
	PRINTSYS_QUE_Main( wk->print_que );

	//2D�`��
	INTRO_GRAPHIC_2D_Draw( wk->graphic );

  // �J�����e�X�g
#ifdef PM_DEBUG
  { 
    static BOOL is_on = TRUE;
 
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
    {
      is_on ^= 1;
    }

    if( is_on )
    {
      GFL_G3D_CAMERA* camera = INTRO_GRAPHIC_GetCamera( wk->graphic );
      debug_camera_test( camera );
    }
  }
#endif

  //3D�`��
  INTRO_GRAPHIC_3D_StartDraw( wk->graphic );

  INTRO_MCSS_Main( wk->mcss );

  INTRO_GRAPHIC_3D_EndDraw( wk->graphic );

	//�R�}���h���s
  is_coutinue = Intro_CMD_Main( wk->cmd );

  // �I������
  if( is_coutinue == FALSE )
  {
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================


#ifdef INTRO_INFOWIN
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
static void Intro_INFOWIN_Init( GAMESYS_WORK *gamesys, HEAPID heapID )
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
static void Intro_INFOWIN_Exit( void )
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
static void Intro_INFOWIN_Main( void )
{	
	INFOWIN_Update();
}
#endif //INTRO_INFOWIN

#ifdef INTRO_TOUCHBAR
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
static TOUCHBAR_WORK * Intro_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heapID )
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
static void Intro_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar )
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
static TOUCHBAR_WORK * Intro_TOUCHBAR_InitEx( INTRO_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID )
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
static void Intro_TOUCHBAR_ExitEx( INTRO_MAIN_WORK *wk )
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
static void Intro_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Main( touchbar );
}
#endif //INTRO_TOUCHBAR

#ifdef INTRO_TASKMENU
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENU�̏�����
 *
 *	@param	menu_res	���\�[�X
 */
//-----------------------------------------------------------------------------
static APP_TASKMENU_WORK * Intro_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heapID )
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
static void Intro_TASKMENU_Exit( APP_TASKMENU_WORK *menu )
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
static void Intro_TASKMENU_Main( APP_TASKMENU_WORK *menu )
{	
	APP_TASKMENU_UpdateMenu( menu );
}
#endif //INTRO_TASKMENU

#ifdef	INTRO_PRINT_TOOL
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
static BOOL PrintTool_MainFunc( INTRO_MAIN_WORK * wk )
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
static void PrintTool_AddBmpWin( INTRO_MAIN_WORK * wk )
{
	wk->print_util.win = GFL_BMPWIN_Create(
													GFL_BG_FRAME0_M,					// �a�f�t���[��
													1, 1,											// �\�����W
													20, 2,										// �\���T�C�Y
													15,												// �p���b�g
													GFL_BMP_CHRAREA_GET_B );	// �L�����擾����
}

// BMPWIN�폜
static void PrintTool_DelBmpWin( INTRO_MAIN_WORK * wk )
{
		GFL_BMPWIN_Delete( wk->print_util.win );
}

// BMPWIN�X�N���[���]��
static void PrintTool_ScreenTrans( INTRO_MAIN_WORK * wk )
{
	GFL_BMPWIN_MakeScreen( wk->print_util.win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(wk->print_util.win) );
}

// �g�o�\��
static void PrintTool_PrintHP( INTRO_MAIN_WORK * wk )
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
#endif	//INTRO_PRINT_TOOL

    

