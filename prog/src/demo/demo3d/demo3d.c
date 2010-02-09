//=============================================================================
/**
 *
 *	@file		demo3d.c
 *	@brief  3D�f���Đ��A�v��
 *	@author	genya hosaka
 *	@data		2009.11.27
 *
 */
//=============================================================================
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

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

//�`��ݒ�
#include "demo3d_graphic.h"

//�A�[�J�C�u
#include "arc_def.h"

//�O�����J
#include "demo/demo3d.h"

#include "message.naix"
#include "townmap_gra.naix"

#include "demo3d_engine.h"

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================
//#define DEMO3D_INFOWIN
//#define DEMO3D_TOUCHBAR
//#define DEMO3D_TASKMENU

FS_EXTERN_OVERLAY(ui_common);

#define CHECK_KEY_TRG( key ) ( ( GFL_UI_KEY_GetTrg() & (key) ) == (key) )

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  DEMO3D_HEAP_SIZE = 0x110000,  ///< �q�[�v�T�C�Y
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
	PLTID_OBJ_TOWNMAP_M	= 14,		

  //�T�uOBJ
  PLTID_OBJ_COMMON_S = 0,
};

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

// �s���S�^
typedef struct _APP_EXCEPTION_WORK APP_EXCEPTION_WORK;

//--------------------------------------------------------------
/// ��O���� �֐���`
//==============================================================
typedef void (*APP_EXCEPTION_FUNC)( APP_EXCEPTION_WORK* wk );

//--------------------------------------------------------------
/// ��O���� �֐��e�[�u��
//==============================================================
typedef struct {
  APP_EXCEPTION_FUNC Init;
  APP_EXCEPTION_FUNC Main;
  APP_EXCEPTION_FUNC End;
} APP_EXCEPTION_FUNC_SET;

//--------------------------------------------------------------
///	�A�v����O����
//==============================================================
struct _APP_EXCEPTION_WORK {
  //[IN]
  HEAPID heapID;
  const DEMO3D_GRAPHIC_WORK* graphic;
  const DEMO3D_ENGINE_WORK* engine;
  //[PRIVATE]
  const APP_EXCEPTION_FUNC_SET* p_funcset;
  void *userwork;
};

//--------------------------------------------------------------
///	BG�Ǘ����[�N
//==============================================================
typedef struct 
{
	int dummy;
} DEMO3D_BG_WORK;

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heapID;

  DEMO3D_PARAM*         param;

	DEMO3D_BG_WORK				wk_bg;

	//�`��ݒ�
	DEMO3D_GRAPHIC_WORK	*graphic;

#ifdef DEMO3D_TOUCHBAR
	//�^�b�`�o�[
	TOUCHBAR_WORK							*touchbar;
	//�ȉ��J�X�^���{�^���g�p����ꍇ�̃T���v�����\�[�X
	u32												ncg_btn;
	u32												ncl_btn;
	u32												nce_btn;
#endif //DEMO3D_TOUCHBAR

	//�t�H���g
	GFL_FONT									*font;

	//�v�����g�L���[
	PRINT_QUE									*print_que;
	GFL_MSGDATA								*msg;

#ifdef DEMO3D_TASKMENU
	//�^�X�N���j���[
	APP_TASKMENU_RES					*menu_res;
	APP_TASKMENU_WORK					*menu;
#endif //DEMO3D_TASKMENU

  // �f���G���W��
  DEMO3D_ENGINE_WORK*   engine;

  // �A�v����O�����G���W��
  APP_EXCEPTION_WORK*   expection;

} DEMO3D_MAIN_WORK;


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
static GFL_PROC_RESULT Demo3DProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT Demo3DProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT Demo3DProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );

#ifdef DEMO3D_INFOWIN
//-------------------------------------
///	INFOWIN
//=====================================
static void Demo3D_INFOWIN_Init( GAMESYS_WORK *gamesys, HEAPID heapID );
static void Demo3D_INFOWIN_Exit( void );
static void Demo3D_INFOWIN_Main( void );
#endif //DEMO3D_INFOWIN

#ifdef DEMO3D_TOUCHBAR
//-------------------------------------
///	�^�b�`�o�[
//=====================================
static TOUCHBAR_WORK * Demo3D_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heapID );
static void Demo3D_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar );
static void Demo3D_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar );
//�ȉ��J�X�^���{�^���g�p�T���v���p
static TOUCHBAR_WORK * Demo3D_TOUCHBAR_InitEx( DEMO3D_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID );
static void Demo3D_TOUCHBAR_ExitEx( DEMO3D_MAIN_WORK *wk );
#endif //DEMO3D_TOUCHBAR

#ifdef DEMO3D_TASKMENU
//-------------------------------------
///	���X�g�V�X�e���͂��A������
//=====================================
static APP_TASKMENU_WORK * Demo3D_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heapID );
static void Demo3D_TASKMENU_Exit( APP_TASKMENU_WORK *menu );
static void Demo3D_TASKMENU_Main( APP_TASKMENU_WORK *menu );
#endif //DEMO3D_TASKMENU

//-----------------------------------------------------------
// �A�v����O����
//-----------------------------------------------------------
static APP_EXCEPTION_WORK* APP_EXCEPTION_Create( DEMO3D_ID demo_id, DEMO3D_GRAPHIC_WORK* graphic, DEMO3D_ENGINE_WORK* engine, HEAPID heapID );
static void APP_EXCEPTION_Delete( APP_EXCEPTION_WORK* wk );
static void APP_EXCEPTION_Main( APP_EXCEPTION_WORK* wk );

//-----------------------------------------------------------
// �V���D�����
//-----------------------------------------------------------
#define C_CRUISER_POS_FLASH_SYNC (100) ///< �X�V�Ԋu

//--------------------------------------------------------------
///	���W�e�[�u��
//==============================================================
static const GFL_CLACTPOS g_c_cruiser_postbl[] = 
{
  { 13*8,  17*8, },		//0F
  { 13*8,  17*8-4, },
  { 13*8,  16*8, },
  { 13*8,  16*8-4, },
  { 13*8,  15*8, },
  { 13*8,  15*8-4, },
  { 13*8,  14*8, },
  { 13*8,  14*8-4, },
  { 13*8,  13*8, },
  { 13*8,  13*8-4, },
  { 13*8,  12*8, },		//1000F	�X�J�C�A���[�u���b�W
  { 13*8,  12*8-4, },
  { 13*8,  11*8, },
  { 13*8,  11*8-4, },
  { 13*8,  10*8, },
  { 13*8,  10*8-4, },
  { 13*8,  9*8, },
  { 13*8,  9*8-4, },
  { 13*8,  8*8, },
  { 13*8,  8*8-4, },
  { 13*8,  7*8, },		//2000F
  { 13*8,  7*8-4, },
  { 13*8,  6*8, },
  { 13*8,  6*8-4, },
  { 13*8,  5*8, },		//2400F	�X�^�[���C��
  { 14*8,  5*8-4, },
  { 15*8,  5*8-4, },
  { 16*8,  5*8, },		//2700F	�܂�Ԃ��n�_
  { 16*8,  6*8-4, },
  { 16*8,  6*8, },
  { 16*8,  7*8-4, },		//3000F
  { 16*8,  7*8, },
  { 16*8,  8*8-4, },
  { 16*8,  8*8, },
  { 16*8,  9*8-4, },
  { 16*8,  9*8, },
  { 16*8,  10*8-4, },
  { 16*8,  10*8, },
  { 16*8,  11*8-4, },
  { 16*8,  11*8, },
  { 16*8,  12*8-4, },		//4000F	�X�J�C�A���[�u���b�W
  { 16*8,  12*8, },
  { 16*8,  13*8-4, },
  { 16*8,  13*8, },
  { 16*8,  14*8-4, },
  { 16*8-4,  14*8, },
  { 15*8,  15*8-4, },
  { 15*8-4,  15*8, },
  { 14*8,  16*8-4, },
  { 14*8-4,  16*8, },
  { 13*8,  17*8-4, },		//5000F
  { 13*8-4,  17*8, },
  { 12*8,  17*8, },
  { 12*8-4,  17*8, },
};

//--------------------------------------------------------------
///	�V���D�p���[�N(��O�����̃��[�U�[���[�N)
//==============================================================
typedef struct {
  u8 pos_id;
  u8 padding[3];
  UI_EASY_CLWK_RES clwk_res;
  GFL_CLWK* clwk_marker;
} EXP_C_CRUISER_WORK;

static void EXP_C_CRUISER_Init( APP_EXCEPTION_WORK* wk );
static void EXP_C_CRUISER_Main( APP_EXCEPTION_WORK* wk );
static void EXP_C_CRUISER_End( APP_EXCEPTION_WORK* wk );

static const APP_EXCEPTION_FUNC_SET c_exp_funcset_c_cruiser = 
{
  EXP_C_CRUISER_Init,
  EXP_C_CRUISER_Main,
  EXP_C_CRUISER_End,
};

//=============================================================================
/**
 *								�O�����J
 */
//=============================================================================
const GFL_PROC_DATA Demo3DProcData = 
{
	Demo3DProc_Init,
	Demo3DProc_Main,
	Demo3DProc_Exit,
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
 *	@param	*pwk					DEMO3D_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT Demo3DProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
	DEMO3D_MAIN_WORK *wk;
	DEMO3D_PARAM *param;

	//�I�[�o�[���C�ǂݍ���
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );
	
	//�����擾
	param	= pwk;

  GF_ASSERT( param->demo_id != DEMO3D_ID_NULL && param->demo_id < DEMO3D_ID_MAX );

	//�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_DEMO3D, DEMO3D_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(DEMO3D_MAIN_WORK), HEAPID_DEMO3D );
  GFL_STD_MemClear( wk, sizeof(DEMO3D_MAIN_WORK) );

  // ������
  wk->heapID      = HEAPID_DEMO3D;
  wk->param       = param;
	
	//�`��ݒ菉����
	wk->graphic	= DEMO3D_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, param->demo_id, wk->heapID );

	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	//���b�Z�[�W
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
			NARC_message_mictest_dat, wk->heapID );

	//PRINT_QUE�쐬
	wk->print_que		= PRINTSYS_QUE_Create( wk->heapID );

  //3D ������
  wk->engine = Demo3D_ENGINE_Init( wk->graphic, param->demo_id, param->start_frame, wk->heapID );

#ifdef DEMO3D_INFOWIN
	//INFOWIN�̏�����
	Demo3D_INFOWIN_Init( param->gamesys, wk->heapID );
#endif //DEMO3D_INFOWIN

#ifdef DEMO3D_TOUCHBAR
	//�^�b�`�o�[�̏�����
	{	
		GFL_CLUNIT	*clunit	= DEMO3D_GRAPHIC_GetClunit( wk->graphic );
		wk->touchbar	= Demo3D_TOUCHBAR_InitEx( wk, clunit, wk->heapID );
	}
#endif //DEMO3D_TOUCHBAR

#ifdef DEMO3D_TASKMENU
	//TASKMENU���\�[�X�ǂݍ��݁�������
	wk->menu_res	= APP_TASKMENU_RES_Create( BG_FRAME_BAR_M, PLTID_BG_TASKMENU_M, wk->font, wk->print_que, wk->heapID );
	wk->menu			= Demo3D_TASKMENU_Init( wk->menu_res, wk->msg, wk->heapID );
#endif //DEMO3D_TASKMENU

  // BG/OBJ���\���ɂ��Ă���
  GFL_BG_SetVisible( BG_FRAME_BACK_S, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_TEXT_S, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_BACK_M, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_POKE_M, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_BAR_M,  VISIBLE_OFF );
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
  
  // �t�F�[�h�C�� ���N�G�X�g
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 2 );

  // �f�����̗�O�����G���W��������
  wk->expection = APP_EXCEPTION_Create( param->demo_id, wk->graphic, wk->engine, wk->heapID );

  return GFL_PROC_RES_FINISH;
}
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC �I������
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					DEMO3D_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT Demo3DProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	DEMO3D_MAIN_WORK* wk = mywk;
  
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

  // ��O�����G���W�� �I������
  APP_EXCEPTION_Delete( wk->expection );

#ifdef DEMO3D_TASKMENU
	//TASKMENU�V�X�e�������\�[�X�j��
	Demo3D_TASKMENU_Exit( wk->menu );
	APP_TASKMENU_RES_Delete( wk->menu_res );	
#endif //DEMO3D_TASKMENU

#ifdef DEMO3D_TOUCHBAR
	//�^�b�`�o�[
	Demo3D_TOUCHBAR_ExitEx( wk );
#endif //DEMO3D_TOUCHBAR

#ifdef DEMO3D_INFOWIN
	//INFWIN
	Demo3D_INFOWIN_Exit();
#endif //DEMO3D_INFOWIN

	//���b�Z�[�W�j��
	GFL_MSG_Delete( wk->msg );

	//PRINT_QUE
	PRINTSYS_QUE_Delete( wk->print_que );

	//FONT
	GFL_FONT_Delete( wk->font );
  
  //3D �j��
  Demo3D_ENGINE_Exit( wk->engine );

	//�`��ݒ�j��
	DEMO3D_GRAPHIC_Exit( wk->graphic );

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
 *	@param	*pwk					DEMO3D_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT Demo3DProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	DEMO3D_MAIN_WORK* wk = mywk;
  BOOL is_end;

  // �t�F�[�h���͏������Ȃ�
  if( GFL_FADE_CheckFade() == TRUE )
  {
    return GFL_PROC_RES_CONTINUE;
  }

  // ����L�[�ŃA�v���I��
  if( 
      GFL_UI_TP_GetTrg() 
#ifndef DEBUG_USE_KEY
      ||
      CHECK_KEY_TRG( PAD_BUTTON_A ) ||
      CHECK_KEY_TRG( PAD_BUTTON_B ) ||
      CHECK_KEY_TRG( PAD_BUTTON_X ) ||
      CHECK_KEY_TRG( PAD_BUTTON_Y ) ||
      CHECK_KEY_TRG( PAD_KEY_LEFT ) ||
      CHECK_KEY_TRG( PAD_KEY_RIGHT ) ||
      CHECK_KEY_TRG( PAD_KEY_DOWN ) ||
      CHECK_KEY_TRG( PAD_KEY_UP ) 
#endif
    )
  {
    wk->param->result = DEMO3D_RESULT_USER_END;
    return GFL_PROC_RES_FINISH;
  }
  
  // ��O�����G���W�� �又��
  APP_EXCEPTION_Main( wk->expection );

#ifdef DEMO3D_TOUCHBAR
	//�^�b�`�o�[���C������
	Demo3D_TOUCHBAR_Main( wk->touchbar );
#endif //DEMO3D_TOUCHBAR

#ifdef DEMO3D_INFOWIN
	//INFWIN���C������
	Demo3D_INFOWIN_Main();
#endif //DEMO3D_INFOWIN

#ifdef DEMO3D_TASKMENU
	//�^�X�N���j���[���C������
	Demo3D_TASKMENU_Main( wk->menu );
#endif //DEMO3D_TASKMENU

	//PRINT_QUE
	PRINTSYS_QUE_Main( wk->print_que );

	//2D�`��
	DEMO3D_GRAPHIC_2D_Draw( wk->graphic );

	//3D�`��
  is_end = Demo3D_ENGINE_Main( wk->engine );

  // ���[�v���o�ŏI��
  if( is_end )
  {
    // [OUT] �t���[���l��ݒ�
    wk->param->end_frame  = DEMO3D_ENGINE_GetNowFrame( wk->engine ) >> FX32_SHIFT; 
    wk->param->result     = DEMO3D_RESULT_FINISH;
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================


#ifdef DEMO3D_INFOWIN
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
static void Demo3D_INFOWIN_Init( GAMESYS_WORK *gamesys, HEAPID heapID )
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
static void Demo3D_INFOWIN_Exit( void )
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
static void Demo3D_INFOWIN_Main( void )
{	
	INFOWIN_Update();
}
#endif //DEMO3D_INFOWIN

#ifdef DEMO3D_TOUCHBAR
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
static TOUCHBAR_WORK * Demo3D_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heapID )
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
static void Demo3D_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar )
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
static TOUCHBAR_WORK * Demo3D_TOUCHBAR_InitEx( DEMO3D_MAIN_WORK *wk, GFL_CLUNIT *clunit, HEAPID heapID )
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
static void Demo3D_TOUCHBAR_ExitEx( DEMO3D_MAIN_WORK *wk )
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
static void Demo3D_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Main( touchbar );
}
#endif //DEMO3D_TOUCHBAR

#ifdef DEMO3D_TASKMENU
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENU�̏�����
 *
 *	@param	menu_res	���\�[�X
 */
//-----------------------------------------------------------------------------
static APP_TASKMENU_WORK * Demo3D_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heapID )
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
static void Demo3D_TASKMENU_Exit( APP_TASKMENU_WORK *menu )
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
static void Demo3D_TASKMENU_Main( APP_TASKMENU_WORK *menu )
{	
	APP_TASKMENU_UpdateMenu( menu );
}
#endif //DEMO3D_TASKMENU

//-----------------------------------------------------------------------------
// �A�v�����Ƃ̗�O�\��
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *	@brief  �A�v����O�����G���W�� ����
 *
 *	@param	DEMO3D_ID demo_id
 *	@param	heapID 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static APP_EXCEPTION_WORK* APP_EXCEPTION_Create( DEMO3D_ID demo_id, DEMO3D_GRAPHIC_WORK* graphic, DEMO3D_ENGINE_WORK* engine, HEAPID heapID )
{
  APP_EXCEPTION_WORK* wk;

  GF_ASSERT( graphic );
  GF_ASSERT( engine );
  
  // ���C�����[�N �A���P�[�g
  wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(APP_EXCEPTION_WORK) );

  // �����o������
  wk->graphic = graphic;
  wk->engine = engine;
  wk->heapID = heapID;

  wk->p_funcset = NULL;

  // �V���D
  switch( demo_id )
  {
  case DEMO3D_ID_C_CRUISER :
    wk->p_funcset = &c_exp_funcset_c_cruiser;
    break;
  default : 
    ;
  }

  // ����������
  if( wk->p_funcset )
  {
    wk->p_funcset->Init( wk );
  }

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �A�v����O�����G���W�� �폜
 *
 *	@param	APP_EXCEPTION_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void APP_EXCEPTION_Delete( APP_EXCEPTION_WORK* wk )
{
  GF_ASSERT(wk);

  // �I������
  if( wk->p_funcset )
  {
    wk->p_funcset->End( wk );
  }

  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �A�v����O�����G���W�� �又��
 *
 *	@param	APP_EXCEPTION_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void APP_EXCEPTION_Main( APP_EXCEPTION_WORK* wk )
{
  GF_ASSERT(wk);

  // �又��
  if( wk->p_funcset )
  {
    wk->p_funcset->Main( wk );
  }
}




//-----------------------------------------------------------------------------
// �V���D �����
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *	@brief  BG�Ǘ����W���[�� ���\�[�X�ǂݍ���
 *
 *	@param	DEMO3D_BG_WORK* wk BG�Ǘ����[�N
 *	@param	heapID  �q�[�vID 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void _C_CRUISER_LoadBG( HEAPID heapID )
{
	ARCHANDLE	*handle;

	handle	= GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, heapID );

	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_townmap_gra_tmap_ship_NCLR,
      PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0, heapID );
	
  GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_townmap_gra_tmap_ship_NCGR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_townmap_gra_tmap_ship_NSCR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );	

	GFL_ARC_CloseDataHandle( handle );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ��O���� �V���D ������
 *
 *	@param	APP_EXCEPTION_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void EXP_C_CRUISER_Init( APP_EXCEPTION_WORK* wk )
{
  EXP_C_CRUISER_WORK* uwk;

  wk->userwork = GFL_HEAP_AllocClearMemory( wk->heapID, sizeof(EXP_C_CRUISER_WORK) );
  
  uwk = wk->userwork;

  _C_CRUISER_LoadBG( wk->heapID );

  {
    GFL_CLUNIT* clunit;
    UI_EASY_CLWK_RES_PARAM prm;

    clunit	= DEMO3D_GRAPHIC_GetClunit( wk->graphic );

    prm.draw_type = CLSYS_DRAW_SUB;
    prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
    prm.arc_id    = ARCID_TOWNMAP_GRAPHIC;
    prm.pltt_id   = NARC_townmap_gra_tmap_ship_obj_NCLR;
    prm.ncg_id    = NARC_townmap_gra_tmap_ship_obj_NCGR;
    prm.cell_id   = NARC_townmap_gra_tmap_ship_obj_NCER;
    prm.anm_id    = NARC_townmap_gra_tmap_ship_obj_NANR;
    prm.pltt_line = PLTID_OBJ_COMMON_S;
    prm.pltt_src_ofs = 0;
    prm.pltt_src_num = 1;

    UI_EASY_CLWK_LoadResource( &uwk->clwk_res, &prm, clunit, wk->heapID );

    uwk->clwk_marker = UI_EASY_CLWK_CreateCLWK( &uwk->clwk_res, clunit, 40, 40, 0, wk->heapID );
      
    GFL_CLACT_WK_SetDrawEnable( uwk->clwk_marker, TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( uwk->clwk_marker, TRUE );

  }
	
  GFL_BG_SetVisible( BG_FRAME_BACK_S, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ��O���� �V���D �又��
 *
 *	@param	APP_EXCEPTION_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void EXP_C_CRUISER_Main( APP_EXCEPTION_WORK* wk )
{
  int frame;
  EXP_C_CRUISER_WORK* uwk = wk->userwork;
  
  frame = DEMO3D_ENGINE_GetNowFrame( wk->engine ) >> FX32_SHIFT;
  if( uwk->pos_id < NELEMS(g_c_cruiser_postbl) )

  {
    if( uwk->pos_id * C_CRUISER_POS_FLASH_SYNC <= frame )
    {
      // ���W�X�V
      GFL_CLACT_WK_SetPos( uwk->clwk_marker, &g_c_cruiser_postbl[ uwk->pos_id ], CLSYS_DRAW_SUB );
      GFL_CLACT_WK_SetAnmFrame( uwk->clwk_marker, 0 );

      OS_TPrintf("frame=%d marker pos_id=%d x=%d, y=%d\n", 
          frame, uwk->pos_id, 
          g_c_cruiser_postbl[uwk->pos_id].x,
          g_c_cruiser_postbl[uwk->pos_id].y );

      uwk->pos_id++;

      // �I������
      if( uwk->pos_id == NELEMS(g_c_cruiser_postbl) )
      {
        // ��\����
        GFL_CLACT_WK_SetDrawEnable( uwk->clwk_marker, FALSE );
        OS_TPrintf("marker visible off \n");
      }
    }
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief  ��O���� �V���D �������
 *
 *	@param	APP_EXCEPTION_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void EXP_C_CRUISER_End( APP_EXCEPTION_WORK* wk )
{
  EXP_C_CRUISER_WORK* uwk = wk->userwork;

  // OBJ���\�[�X�J��
  UI_EASY_CLWK_UnLoadResource( &uwk->clwk_res );

  // ���[�U�[���[�N���
  GFL_HEAP_FreeMemory( wk->userwork );
}

