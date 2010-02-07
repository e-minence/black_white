//=============================================================================
/**
 *
 *	@file		un_select.c
 *	@brief  ���A �t���A�I��
 *	@author	genya hosaka
 *	@data		2010.02.05
 *
 */
//=============================================================================
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/bmp_winframe.h" // for BmpWinFrame_
#include "system/brightness.h" // for

// �ȈՉ�b�\���V�X�e��
#include "system/pms_draw.h"

#include "gamesystem/msgspeed.h"  // for MSGSPEED_GetWait

// ������֘A
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h"
#include "font/font.naix"

//INFOWIN
#include "infowin/infowin.h"

//�`��ݒ�
#include "un_select_graphic.h"

//�Ȉ�CLWK�ǂݍ��݁��J�����[�e�B���e�B�[
#include "ui/ui_easy_clwk.h"

//�^�b�`�o�[
#include "ui/touchbar.h"

// �V�[���R���g���[���[
#include "ui/ui_scene.h"

//�^�X�N���j���[
#include "app/app_taskmenu.h"

//�A�v�����ʑf��
#include "app/app_menu_common.h"

//�A�[�J�C�u
#include "arc_def.h"

//�O�����J
#include "app/un_select.h"

#include "message.naix"
#include "un_select_gra.naix"	// �A�[�J�C�u

// �T�E���h
#include "sound/pm_sndsys.h"

//@TODO
#include "msg/msg_mictest.h"  // GMM

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================
#define UN_SELECT_TOUCHBAR

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  UN_SELECT_HEAP_SIZE = 0x30000,  ///< �q�[�v�T�C�Y
};

// �p�b�V�u�萔
#define YESNO_MASK_DISP ( MASK_DOUBLE_DISPLAY )
#define YESNO_MASK_PLANE ( PLANEMASK_BG2 | PLANEMASK_BG3 | PLANEMASK_OBJ | PLANEMASK_BD )
#define BRIGHT_PASSIVE_SYNC (8)
#define BRIGHT_PASSIVE_VOL (-8)

//--------------------------------------------------------------
///	�A�v���I�����[�h
//==============================================================
typedef enum { 
  END_MODE_DECIDE,
  END_MODE_CANCEL
} END_MODE;

//-------------------------------------
///	�t���[��
//=====================================
enum
{	
	BG_FRAME_TEXT_M	= GFL_BG_FRAME0_M,
	BG_FRAME_MENU_M	= GFL_BG_FRAME1_M,
	BG_FRAME_LIST_M	= GFL_BG_FRAME2_M,
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
	PLTID_BG_TEXT_M				= 1,
  PLTID_BG_TEXT_WIN_M   = 2,
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
} UN_SELECT_BG_WORK;

//--------------------------------------------------------------
///	���b�Z�[�W�Ǘ����[�N
//==============================================================
typedef struct
{ 
  //[IN]
  HEAPID  heap_id;

  //[PRIVATE]
  GFL_TCBLSYS   *tcblsys;     ///< �^�X�N�V�X�e��
  GFL_MSGDATA   *msghandle;   ///< ���b�Z�[�W�n���h��
	GFL_FONT			*font;        ///< �t�H���g
	PRINT_QUE			*print_que;   ///< �v�����g�L���[
  WORDSET       *wordset;     ///< ���[�h�Z�b�g

  STRBUF        *strbuf;
  STRBUF        *exp_strbuf;

  // �X�g���[���Đ�
  PRINT_STREAM* print_stream;
  GFL_BMPWIN    *win_talk;

} UN_SELECT_MSG_CNT_WORK;

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heap_id;

	UN_SELECT_BG_WORK				wk_bg;

	//�`��ݒ�
	UN_SELECT_GRAPHIC_WORK	*graphic;

#ifdef UN_SELECT_TOUCHBAR
	//�^�b�`�o�[
	TOUCHBAR_WORK							*touchbar;
	//�ȉ��J�X�^���{�^���g�p����ꍇ�̃T���v�����\�[�X
	u32												ncg_btn;
	u32												ncl_btn;
	u32												nce_btn;
#endif //UN_SELECT_TOUCHBAR
	
  //�^�X�N���j���[
	APP_TASKMENU_RES					*menu_res;
	APP_TASKMENU_WORK					*menu;

  UN_SELECT_PARAM* pwk;

  UI_SCENE_CNT_PTR cnt_scene;

  UN_SELECT_MSG_CNT_WORK* cnt_msg;

} UN_SELECT_MAIN_WORK;

//=============================================================================
/**
 *							�V�[����`
 */
//=============================================================================

// �t���A�I��
static BOOL SceneSelectFloor_Init( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneSelectFloor_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneSelectFloor_End( UI_SCENE_CNT_PTR cnt, void* work );

// �m�F���
static BOOL SceneConfirm_Init( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneConfirm_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneConfirm_End( UI_SCENE_CNT_PTR cnt, void* work );

//--------------------------------------------------------------
///	SceneID
//==============================================================
typedef enum
{ 
  UNS_SCENE_ID_SELECT_FLOOR = 0,  ///< �t���A�I��
  UNS_SCENE_ID_CONFIRM,           ///< �m�F���

  UNS_SCENE_ID_MAX,
} UNS_SCENE_ID;

//--------------------------------------------------------------
///	SceneFunc
//==============================================================
static const UI_SCENE_FUNC_SET c_scene_func_tbl[ UNS_SCENE_ID_MAX ] = 
{
  // UNS_SCENE_ID_SELECT_FLOOR
  {
    SceneSelectFloor_Init,
    NULL,
    SceneSelectFloor_Main,
    NULL,
    SceneSelectFloor_End,
  },
  // UNS_SCENE_ID_CONFIRM
  {
    SceneConfirm_Init,
    NULL,
    SceneConfirm_Main,
    NULL,
    SceneConfirm_End,
  },
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
static GFL_PROC_RESULT UNSelectProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT UNSelectProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT UNSelectProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
//-------------------------------------
///	�ėp�������[�e�B���e�B
//=====================================
static void UNSelect_BG_LoadResource( UN_SELECT_BG_WORK* wk, HEAPID heap_id );

#ifdef UN_SELECT_TOUCHBAR
//-------------------------------------
///	�^�b�`�o�[
//=====================================
static TOUCHBAR_WORK * UNSelect_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heap_id );
static void UNSelect_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar );
static void UNSelect_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar );
#endif //UN_SELECT_TOUCHBAR

//-------------------------------------
///	���X�g�V�X�e���͂��A������
//=====================================
static APP_TASKMENU_WORK * UNSelect_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heap_id );
static void UNSelect_TASKMENU_Exit( APP_TASKMENU_WORK *menu );
static void UNSelect_TASKMENU_Main( APP_TASKMENU_WORK *menu );

//-------------------------------------
// �p�b�V�u
//=====================================
static void PASSIVE_Request( void );
static void PASSIVE_Reset( void );

//-------------------------------------
// ���b�Z�[�W�Ǘ����W���[��
//=====================================
static UN_SELECT_MSG_CNT_WORK* MSG_CNT_Create( HEAPID heap_id );
static void MSG_CNT_Delete( UN_SELECT_MSG_CNT_WORK* wk );
static void MSG_CNT_Main( UN_SELECT_MSG_CNT_WORK* wk );
static void MSG_CNT_SetPrint( UN_SELECT_MSG_CNT_WORK* wk, int str_id );
static BOOL MSG_CNT_PrintProc( UN_SELECT_MSG_CNT_WORK* wk );
static void MSG_CNT_PrintClear( UN_SELECT_MSG_CNT_WORK* wk );
static GFL_FONT* MSG_CNT_GetFont( UN_SELECT_MSG_CNT_WORK* wk );
static PRINT_QUE* MSG_CNT_GetPrintQue( UN_SELECT_MSG_CNT_WORK* wk );
static GFL_MSGDATA* MSG_CNT_GetMsgData( UN_SELECT_MSG_CNT_WORK* wk );

// PROTOTYPE
static UN_SELECT_MAIN_WORK* app_init( GFL_PROC* proc, UN_SELECT_PARAM* prm );
static void app_end( UN_SELECT_MAIN_WORK* wk, END_MODE end_mode );

//=============================================================================
/**
 *								�O�����J
 */
//=============================================================================
const GFL_PROC_DATA UNSelectProcData = 
{
	UNSelectProc_Init,
	UNSelectProc_Main,
	UNSelectProc_Exit,
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
 *	@param	*pwk					UN_SELECT_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UNSelectProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
	UN_SELECT_MAIN_WORK *wk;
	
  GF_ASSERT( pwk );

  switch( *seq )
  {
  case 0:
    wk = app_init( proc, pwk );
  
    // �t�F�[�h�C�� ���N�G�X�g
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1 );

    (*seq)++;
    break;

  case 1:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      return GFL_PROC_RES_FINISH;
    }
    break;

  default : GF_ASSERT(0);
  }

  return GFL_PROC_RES_CONTINUE;
}
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC �I������
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					UN_SELECT_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UNSelectProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	UN_SELECT_MAIN_WORK* wk = mywk;
    
  if( GFL_FADE_CheckFade() == TRUE )
  {
     return GFL_PROC_RES_CONTINUE;
  }
  
  // �V�[���R���g�[���폜
  UI_SCENE_CNT_Delete( wk->cnt_scene );

  // ���b�Z�[�W����
  MSG_CNT_Delete( wk->cnt_msg );

	//TASKMENU�V�X�e�������\�[�X�j��
	UNSelect_TASKMENU_Exit( wk->menu );
	APP_TASKMENU_RES_Delete( wk->menu_res );	

#ifdef UN_SELECT_TOUCHBAR
	//�^�b�`�o�[
	UNSelect_TOUCHBAR_Exit( wk->touchbar );
#endif //UN_SELECT_TOUCHBAR

	//�`��ݒ�j��
	UN_SELECT_GRAPHIC_Exit( wk->graphic );

	//PROC�p���������
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( wk->heap_id );

	//�I�[�o�[���C�j��
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

  HOSAKA_Printf(" PROC�I���I \n");

  return GFL_PROC_RES_FINISH;
}
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC �又��
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					UN_SELECT_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UNSelectProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	UN_SELECT_MAIN_WORK* wk = mywk;
  
  // SCENE
  if( UI_SCENE_CNT_Main( wk->cnt_scene ) )
  {
    return GFL_PROC_RES_FINISH;
  }

  // ���b�Z�[�W�又��
  MSG_CNT_Main( wk->cnt_msg );

	//2D�`��
	UN_SELECT_GRAPHIC_2D_Draw( wk->graphic );

	//3D�`��
	UN_SELECT_GRAPHIC_3D_StartDraw( wk->graphic );

	UN_SELECT_GRAPHIC_3D_EndDraw( wk->graphic );

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
 *	@param	UN_SELECT_BG_WORK* wk BG�Ǘ����[�N
 *	@param	heap_id  �q�[�vID 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void UNSelect_BG_LoadResource( UN_SELECT_BG_WORK* wk, HEAPID heap_id )
{
	ARCHANDLE	*handle;

	handle	= GFL_ARC_OpenDataHandle( ARCID_UN_SELECT_GRA, heap_id );

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_un_select_gra_kokuren_bg_NCLR, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, 0x20, heap_id );
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_un_select_gra_kokuren_bg_NCLR, PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0x20, heap_id );
	
  //	----- ����� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_un_select_gra_kokuren_bg_NCGR,
						BG_FRAME_BACK_S, 0, 0, 0, heap_id );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_un_select_gra_kokuren_bgu_NSCR,
						BG_FRAME_BACK_S, 0, 0, 0, heap_id );	

	//	----- ���� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_un_select_gra_kokuren_bg_NCGR,
						BG_FRAME_BACK_M, 0, 0, 0, heap_id );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_un_select_gra_kokuren_bgd_NSCR,
						BG_FRAME_BACK_M, 0, 0, 0, heap_id );		

	GFL_ARC_CloseDataHandle( handle );
}


#ifdef UN_SELECT_TOUCHBAR
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
static TOUCHBAR_WORK * UNSelect_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heap_id )
{	
  TOUCHBAR_WORK* touchbar;

	//�A�C�R���̐ݒ�
	//�������
	TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]	=
	{	
		{	
			TOUCHBAR_ICON_RETURN,
			{	TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
		},
		{	
			TOUCHBAR_ICON_CUR_L,
			{	TOUCHBAR_ICON_X_00, TOUCHBAR_ICON_Y },
		},
		{	
			TOUCHBAR_ICON_CUR_R,
			{	TOUCHBAR_ICON_X_02, TOUCHBAR_ICON_Y },
		},
	};

	//�ݒ�\����
	//�����قǂ̑����{���\�[�X���������
	TOUCHBAR_SETUP	touchbar_setup;
	GFL_STD_MemClear( &touchbar_setup, sizeof(TOUCHBAR_SETUP) );

	touchbar_setup.p_item		= touchbar_icon_tbl;				//��̑����
	touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);//�����������邩
	touchbar_setup.p_unit		= clunit;										//OBJ�ǂݍ��݂̂��߂�CLUNIT
	touchbar_setup.bar_frm	= BG_FRAME_MENU_M;						//BG�ǂݍ��݂̂��߂�BG��
	touchbar_setup.bg_plt		= PLTID_BG_TOUCHBAR_M;			//BG��گ�
	touchbar_setup.obj_plt	= PLTID_OBJ_TOUCHBAR_M;			//OBJ��گ�
	touchbar_setup.mapping	= APP_COMMON_MAPPING_128K;	//�}�b�s���O���[�h

	touchbar =  TOUCHBAR_Init( &touchbar_setup, heap_id );

  TOUCHBAR_SetBGPriorityAll( touchbar, 1 ); // BG�v���C�I���e�B��ݒ�

  return touchbar;
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR�j��
 *
 *	@param	TOUCHBAR_WORK	*touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void UNSelect_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Exit( touchbar );
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR���C������
 *
 *	@param	TOUCHBAR_WORK	*touchbar �^�b�`�o�[
 */
//-----------------------------------------------------------------------------
static void UNSelect_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Main( touchbar );
}
#endif //UN_SELECT_TOUCHBAR

//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENU�̏�����
 *
 *	@param	menu_res	���\�[�X
 */
//-----------------------------------------------------------------------------
static APP_TASKMENU_WORK * UNSelect_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heap_id )
{	
	int i;
	APP_TASKMENU_INITWORK	init;
	APP_TASKMENU_ITEMWORK	item[2];
	APP_TASKMENU_WORK			*menu;	

	//���̐ݒ�
	for( i = 0; i < NELEMS(item); i++ )
	{	
		item[i].str	      = GFL_MSG_CreateString( msg, 0 );	//������
		item[i].msgColor	= APP_TASKMENU_ITEM_MSGCOLOR;	//�����F
		item[i].type			= APP_TASKMENU_WIN_TYPE_NORMAL;	//���̎��
	}

	//������
	init.heapId		= heap_id;
	init.itemNum	= NELEMS(item);
	init.itemWork = item;
	init.posType	= ATPT_RIGHT_DOWN;
	init.charPosX	= 32;
	init.charPosY = 18;
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
static void UNSelect_TASKMENU_Exit( APP_TASKMENU_WORK *menu )
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
static void UNSelect_TASKMENU_Main( APP_TASKMENU_WORK *menu )
{	
	APP_TASKMENU_UpdateMenu( menu );
}


//-----------------------------------------------------------------------------
/**
 *	@brief  �p�b�V�u���N�G�X�g
 *
 *	@param	void 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PASSIVE_Request( void )
{
  BrightnessChgReset( YESNO_MASK_DISP );
  ChangeBrightnessRequest( BRIGHT_PASSIVE_SYNC, BRIGHT_PASSIVE_VOL, 0, YESNO_MASK_PLANE, YESNO_MASK_DISP );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �p�b�V�u���Z�b�g
 *
 *	@param	void 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void PASSIVE_Reset( void )
{
  BrightnessChgReset( YESNO_MASK_DISP ); // �p�b�V�u����
  ChangeBrightnessRequest( BRIGHT_PASSIVE_SYNC, 0, BRIGHT_PASSIVE_VOL, YESNO_MASK_PLANE, YESNO_MASK_DISP );
}


//=============================================================================
//
//
// ���b�Z�[�W�Ǘ��N���X
//
//
//=============================================================================
enum
{ 
  CGX_BMPWIN_FRAME_POS = 1,
  STRBUF_SIZE = 1600,
};

#define MSG_CNT_SKIP_BTN (PAD_BUTTON_A|PAD_BUTTON_B)

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�Ǘ��N���X ����
 *
 *	@param	HEAPID heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static UN_SELECT_MSG_CNT_WORK* MSG_CNT_Create( HEAPID heap_id )
{
  UN_SELECT_MSG_CNT_WORK* wk;

  // ������ �A���b�N
  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof( UN_SELECT_MSG_CNT_WORK ) );

  // �����o������
  wk->heap_id = heap_id;
  
  // ���b�Z�[�W�p�t�H���g�]��
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0x20*PLTID_BG_TEXT_M, 0x20, heap_id );
  
  // �t���[���E�B���h�E�p�̃L������p��
  BmpWinFrame_GraphicSet( BG_FRAME_TEXT_M, CGX_BMPWIN_FRAME_POS, PLTID_BG_TEXT_WIN_M, MENU_TYPE_FIELD, heap_id );
  
  // ���[�h�Z�b�g����
  wk->wordset = WORDSET_Create( heap_id );

	//���b�Z�[�W
	wk->msghandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
			NARC_message_mictest_dat, heap_id );
  //@TODO �}�C�N�e�X�g

	//PRINT_QUE�쐬
	wk->print_que		= PRINTSYS_QUE_Create( heap_id );

	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, heap_id );

  // STRBU ����
  wk->strbuf      = GFL_STR_CreateBuffer( STRBUF_SIZE, heap_id );
  wk->exp_strbuf  = GFL_STR_CreateBuffer( STRBUF_SIZE, heap_id );

  wk->tcblsys = GFL_TCBL_Init( heap_id, heap_id, 1, 0 );
  
  // �E�B���h�E����
  wk->win_talk = GFL_BMPWIN_Create( BG_FRAME_TEXT_M, 1, 19, 30, 4, PLTID_BG_TEXT_M, GFL_BMP_CHRAREA_GET_B );
  
  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�Ǘ��N���X �j��
 *
 *	@param	UN_SELECT_MSG_CNT_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void MSG_CNT_Delete( UN_SELECT_MSG_CNT_WORK* wk )
{
  // �X�g���[���j��
  if( wk->print_stream )
  {
    PRINTSYS_PrintStreamDelete( wk->print_stream );
    wk->print_stream = NULL;
  }

  // BMPWIN �j��
  GFL_BMPWIN_Delete( wk->win_talk );
  // STRBUF �j��
  GFL_STR_DeleteBuffer( wk->strbuf );
  GFL_STR_DeleteBuffer( wk->exp_strbuf );
	//���b�Z�[�W�j��
	GFL_MSG_Delete( wk->msghandle );
	//PRINT_QUE
	PRINTSYS_QUE_Delete( wk->print_que );
	//FONT
	GFL_FONT_Delete( wk->font );
  // TCBL
  GFL_TCBL_Exit( wk->tcblsys );
  // ���[�h�Z�b�g �j��
  WORDSET_Delete( wk->wordset );

  // �������j��
  GFL_HEAP_FreeMemory( wk );
}


//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�Ǘ��N���X �又��
 *
 *	@param	UN_SELECT_MSG_CNT_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void MSG_CNT_Main( UN_SELECT_MSG_CNT_WORK* wk )
{
	PRINTSYS_QUE_Main( wk->print_que );
  
  GFL_TCBL_Main( wk->tcblsys );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�Ǘ��N���X ���b�Z�[�W�v�����g ���N�G�X�g
 *
 *	@param	UN_SELECT_MSG_CNT_WORK* wk
 *	@param	str_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void MSG_CNT_SetPrint( UN_SELECT_MSG_CNT_WORK* wk, int str_id )
{
  const u8 clear_color = 15;
  GFL_BMPWIN* win;
  int msgspeed;

  GF_ASSERT(wk);
  
  msgspeed  = MSGSPEED_GetWait();
  win       = wk->win_talk;
  
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(win), clear_color);
  GFL_FONTSYS_SetColor(1, 2, clear_color);

  GFL_MSG_GetString( wk->msghandle, str_id, wk->strbuf );

#ifdef PM_DEBUG
  GFL_STR_CheckBufferValid( wk->strbuf ); ///< �j���`�F�b�N
#endif
  
  WORDSET_ExpandStr( wk->wordset, wk->exp_strbuf, wk->strbuf );

#ifdef PM_DEBUG
  GFL_STR_CheckBufferValid( wk->strbuf ); ///< �j���`�F�b�N
#endif

  wk->print_stream = PRINTSYS_PrintStream( win, 4, 0, wk->exp_strbuf, wk->font, msgspeed,
                                           wk->tcblsys, 0xffff, wk->heap_id, clear_color );
  
  // �t���[������
  BmpWinFrame_Write( wk->win_talk, WINDOW_TRANS_OFF, CGX_BMPWIN_FRAME_POS, GFL_BMPWIN_GetPalette(wk->win_talk) );

  // �]��
  { 
    GFL_BMPWIN_TransVramCharacter( win );
    GFL_BMPWIN_MakeScreen( win );

    GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(win) );
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�Ǘ��N���X ���b�Z�[�W����
 *
 *	@param	UN_SELECT_MSG_CNT_WORK* wk 
 *
 *	@retval TRUE:�v�����g�I��
 */
//-----------------------------------------------------------------------------
static BOOL MSG_CNT_PrintProc( UN_SELECT_MSG_CNT_WORK* wk )
{
  PRINTSTREAM_STATE state;

  GF_ASSERT(wk);

//  HOSAKA_Printf("print state= %d \n", state );

  if( wk->print_stream )
  {
    state = PRINTSYS_PrintStreamGetState( wk->print_stream );

    switch( state )
    {
    case PRINTSTREAM_STATE_DONE : // �I��
      PRINTSYS_PrintStreamDelete( wk->print_stream );
      wk->print_stream = NULL;
      return TRUE;

    case PRINTSTREAM_STATE_PAUSE : // �ꎞ��~��
      // �L�[���͑҂�
      if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE || (  GFL_UI_KEY_GetTrg() == PAD_BUTTON_CANCEL ) || GFL_UI_TP_GetTrg() )
      {
        PRINTSYS_PrintStreamReleasePause( wk->print_stream );
        GFL_SOUND_PlaySE( SEQ_SE_DECIDE1 );
      }
      break;

    case PRINTSTREAM_STATE_RUNNING :  // ���s��
      // ���b�Z�[�W�X�L�b�v
      if( (GFL_UI_KEY_GetCont() & MSG_CNT_SKIP_BTN) || GFL_UI_TP_GetCont() )
      {
        PRINTSYS_PrintStreamShortWait( wk->print_stream, 0 );
      }
      break;

    default : GF_ASSERT(0);
    }

    return FALSE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�t���[�����N���A
 *
 *	@param	UN_SELECT_MSG_CNT_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void MSG_CNT_PrintClear( UN_SELECT_MSG_CNT_WORK* wk )
{
  BmpWinFrame_Clear( wk->win_talk, WINDOW_TRANS_ON_V );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�Ǘ��N���X �t�H���g�擾
 *
 *	@param	UN_SELECT_MSG_CNT_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GFL_FONT* MSG_CNT_GetFont( UN_SELECT_MSG_CNT_WORK* wk )
{
  GF_ASSERT(wk);

  return wk->font;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�Ǘ��N���X �v�����g�L���[�擾
 *
 *	@param	UN_SELECT_MSG_CNT_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static PRINT_QUE* MSG_CNT_GetPrintQue( UN_SELECT_MSG_CNT_WORK* wk )
{
  GF_ASSERT(wk);

  return wk->print_que;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�Ǘ��N���X ���b�Z�[�W�n���h�����擾
 *
 *	@param	UN_SELECT_MSG_CNT_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GFL_MSGDATA* MSG_CNT_GetMsgData( UN_SELECT_MSG_CNT_WORK* wk )
{
  GF_ASSERT(wk);

  return wk->msghandle;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  PROC ������
 *
 *	@param	GFL_PROC* proc
 *	@param	prm 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static UN_SELECT_MAIN_WORK* app_init( GFL_PROC* proc, UN_SELECT_PARAM* prm )
{
  UN_SELECT_MAIN_WORK* wk;

	//�I�[�o�[���C�ǂݍ���
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	
	//�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_UN_SELECT, UN_SELECT_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(UN_SELECT_MAIN_WORK), HEAPID_UN_SELECT );
  GFL_STD_MemClear( wk, sizeof(UN_SELECT_MAIN_WORK) );

  // ������
  wk->heap_id = HEAPID_UN_SELECT;
  wk->pwk = prm;
	
	//�`��ݒ菉����
	wk->graphic	= UN_SELECT_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heap_id );

  // ���b�Z�[�W����
  wk->cnt_msg = MSG_CNT_Create( wk->heap_id );
  
  // �V�[���R���g���[���쐬
  wk->cnt_scene = UI_SCENE_CNT_Create( 
      wk->heap_id, c_scene_func_tbl, UNS_SCENE_ID_MAX, 
      UNS_SCENE_ID_SELECT_FLOOR, wk );

	//BG���\�[�X�ǂݍ���
	UNSelect_BG_LoadResource( &wk->wk_bg, wk->heap_id );

#ifdef UN_SELECT_TOUCHBAR
	//�^�b�`�o�[�̏�����
	{	
		GFL_CLUNIT	*clunit	= UN_SELECT_GRAPHIC_GetClunit( wk->graphic );
		wk->touchbar	= UNSelect_TOUCHBAR_Init( clunit, wk->heap_id );
	}
#endif //UN_SELECT_TOUCHBAR

	//TASKMENU���\�[�X�ǂݍ��݁�������
	wk->menu_res	= APP_TASKMENU_RES_Create( BG_FRAME_MENU_M, PLTID_BG_TASKMENU_M,
      MSG_CNT_GetFont( wk->cnt_msg ),
      MSG_CNT_GetPrintQue( wk->cnt_msg ),
      wk->heap_id );

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �A�v���P�[�V�����I������
 *
 *	@param	UN_SELECT_MAIN_WORK* wk
 *	@param	end_mode 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void app_end( UN_SELECT_MAIN_WORK* wk, END_MODE end_mode )
{
  switch( end_mode )
  {
  case END_MODE_DECIDE: 
      // �f�[�^�f���o��
      //@TODO �I���t���A
    break;
  case END_MODE_CANCEL:
      // �f�[�^�f���o��
      //@TODO �L�����Z��
    break;
  }
  
  // �t�F�[�h�A�E�g ���N�G�X�g
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1 );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  SCENE �t���A�I�� ����������
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneSelectFloor_Init( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  SCENE �t���A�I�� �又��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneSelectFloor_Main( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;

  // �t���A�I������
  // @TODO �^�b�`�y�уJ�[�\���őI��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    UI_SCENE_CNT_SetNextScene( cnt, UNS_SCENE_ID_CONFIRM );
    return TRUE;
  }

#ifdef UN_SELECT_TOUCHBAR
	//�^�b�`�o�[���C������
	UNSelect_TOUCHBAR_Main( wk->touchbar );
#endif //UN_SELECT_TOUCHBAR

  switch( TOUCHBAR_GetTrg( wk->touchbar ) )
  {
  case TOUCHBAR_ICON_CUR_L : // ��
    //@TODO
    HOSAKA_Printf("L!\n");
    break;

  case TOUCHBAR_ICON_CUR_R : // ��
    //@TODO
    HOSAKA_Printf("R!\n"); 
    break;

  case TOUCHBAR_ICON_RETURN : // ���^�[���{�^��
    // �I��
    app_end( wk, END_MODE_CANCEL ); 
    UI_SCENE_CNT_SetNextScene( cnt, UI_SCENE_ID_END );
    break;

  case TOUCHBAR_SELECT_NONE : // �I���Ȃ�
    break;

  default : GF_ASSERT(0);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  SCENE �t���A�I�� �㏈��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneSelectFloor_End( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  SCENE �m�F��� ����������
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneConfirm_Init( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;
  u8 seq = UI_SCENE_CNT_GetSubSeq( cnt );

  switch(seq)
  {
  case 0:
		// �p�b�V�u��ԂɑJ��
    PASSIVE_Request();

#if 0
    TOUCHBAR_SetVisible( wk->touchbar, TOUCHBAR_ICON_CUR_L, FALSE );
    TOUCHBAR_SetVisible( wk->touchbar, TOUCHBAR_ICON_CUR_R, FALSE );
    TOUCHBAR_SetVisible( wk->touchbar, TOUCHBAR_ICON_RETURN, FALSE );
#endif

    //@TODO str_id
    MSG_CNT_SetPrint( wk->cnt_msg, 0 );
    UI_SCENE_CNT_IncSubSeq( cnt );
    break;

  case 1:
    if( MSG_CNT_PrintProc(wk->cnt_msg) )
    {
      UI_SCENE_CNT_IncSubSeq( cnt );
    }
    break;

  case 2:
    // �^�X�N���j���[�\��
    wk->menu = UNSelect_TASKMENU_Init( 
        wk->menu_res, 
        MSG_CNT_GetMsgData(wk->cnt_msg), 
        wk->heap_id );

    return TRUE;

  default : GF_ASSERT(0);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  SCENE �m�F��� �又��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneConfirm_Main( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;

	//�^�X�N���j���[���C������
	UNSelect_TASKMENU_Main( wk->menu );

  if( APP_TASKMENU_IsFinish( wk->menu ) )
  {
    u8 pos = APP_TASKMENU_GetCursorPos( wk->menu );

    switch( pos )
    {
    case 0 :
      // �I��
      app_end( wk, END_MODE_DECIDE );
      UI_SCENE_CNT_SetNextScene( cnt, UI_SCENE_ID_END );
      break;
    case 1 :
      // �I����ʂɖ߂�
      PASSIVE_Reset();
      MSG_CNT_PrintClear( wk->cnt_msg );
      UNSelect_TASKMENU_Exit( wk->menu );
      UI_SCENE_CNT_SetNextScene( cnt, UNS_SCENE_ID_SELECT_FLOOR );
      break;
    default : GF_ASSERT(0);
    }

    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  SCENE �m�F��� �㏈��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneConfirm_End( UI_SCENE_CNT_PTR cnt, void* work )
{
  UN_SELECT_MAIN_WORK* wk = work;

  return TRUE;
}

