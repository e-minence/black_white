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
#include "un_select_graphic.h"

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

//�A�[�J�C�u
#include "arc_def.h"

//�O�����J
#include "app/un_select.h"


#include "message.naix"
#include "un_select_gra.naix"	// �A�[�J�C�u

//@TODO
#include "msg/msg_mictest.h"  // GMM

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================
#define UN_SELECT_TOUCHBAR
#define UN_SELECT_TASKMENU

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
} UN_SELECT_BG_WORK;

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heapID;

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

	//�t�H���g
	GFL_FONT									*font;

	//�v�����g�L���[
	PRINT_QUE									*print_que;
	GFL_MSGDATA								*msg;

#ifdef UN_SELECT_TASKMENU
	//�^�X�N���j���[
	APP_TASKMENU_RES					*menu_res;
	APP_TASKMENU_WORK					*menu;
#endif //UN_SELECT_TASKMENU

} UN_SELECT_MAIN_WORK;


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
static GFL_PROC_RESULT UNSelectProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT UNSelectProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT UNSelectProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
//-------------------------------------
///	�ėp�������[�e�B���e�B
//=====================================
static void UNSelect_BG_LoadResource( UN_SELECT_BG_WORK* wk, HEAPID heapID );

#ifdef UN_SELECT_TOUCHBAR
//-------------------------------------
///	�^�b�`�o�[
//=====================================
static TOUCHBAR_WORK * UNSelect_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heapID );
static void UNSelect_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar );
static void UNSelect_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar );
#endif //UN_SELECT_TOUCHBAR

#ifdef UN_SELECT_TASKMENU
//-------------------------------------
///	���X�g�V�X�e���͂��A������
//=====================================
static APP_TASKMENU_WORK * UNSelect_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heapID );
static void UNSelect_TASKMENU_Exit( APP_TASKMENU_WORK *menu );
static void UNSelect_TASKMENU_Main( APP_TASKMENU_WORK *menu );
#endif //UN_SELECT_TASKMENU

static UN_SELECT_MAIN_WORK* proc_init( GFL_PROC* proc, UN_SELECT_PARAM* prm );

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
    wk = proc_init( proc, pwk );
  
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

#ifdef UN_SELECT_TASKMENU
	//TASKMENU�V�X�e�������\�[�X�j��
	UNSelect_TASKMENU_Exit( wk->menu );
	APP_TASKMENU_RES_Delete( wk->menu_res );	
#endif //UN_SELECT_TASKMENU

#ifdef UN_SELECT_TOUCHBAR
	//�^�b�`�o�[
	UNSelect_TOUCHBAR_Exit( wk->touchbar );
#endif //UN_SELECT_TOUCHBAR

	//���b�Z�[�W�j��
	GFL_MSG_Delete( wk->msg );

	//PRINT_QUE
	PRINTSYS_QUE_Delete( wk->print_que );

	//FONT
	GFL_FONT_Delete( wk->font );

	//�`��ݒ�j��
	UN_SELECT_GRAPHIC_Exit( wk->graphic );

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
 *	@param	*pwk					UN_SELECT_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UNSelectProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	UN_SELECT_MAIN_WORK* wk = mywk;

  // �f�o�b�O�{�^���ŃA�v���I��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG )
  {
    // �t�F�[�h�A�E�g ���N�G�X�g
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1 );
    return GFL_PROC_RES_FINISH;
  }

#ifdef UN_SELECT_TOUCHBAR
	//�^�b�`�o�[���C������
	UNSelect_TOUCHBAR_Main( wk->touchbar );
#endif //UN_SELECT_TOUCHBAR

#ifdef UN_SELECT_TASKMENU
	//�^�X�N���j���[���C������
	UNSelect_TASKMENU_Main( wk->menu );
#endif //UN_SELECT_TASKMENU

	//PRINT_QUE
	PRINTSYS_QUE_Main( wk->print_que );

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
 *	@param	heapID  �q�[�vID 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void UNSelect_BG_LoadResource( UN_SELECT_BG_WORK* wk, HEAPID heapID )
{
	ARCHANDLE	*handle;

	handle	= GFL_ARC_OpenDataHandle( ARCID_UN_SELECT_GRA, heapID );

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_un_select_gra_kokuren_bg_NCLR, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, 0x20, heapID );
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_un_select_gra_kokuren_bg_NCLR, PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0x20, heapID );
	
  //	----- ����� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_un_select_gra_kokuren_bg_NCGR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_un_select_gra_kokuren_bgu_NSCR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );	

	//	----- ���� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_un_select_gra_kokuren_bg_NCGR,
						BG_FRAME_BACK_M, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_un_select_gra_kokuren_bgd_NSCR,
						BG_FRAME_BACK_M, 0, 0, 0, heapID );		

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
 *	@param	heapID							�q�[�vID
 *
 *	@return	TOUCHBAR_WORK
 */
//-----------------------------------------------------------------------------
static TOUCHBAR_WORK * UNSelect_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heapID )
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

#ifdef UN_SELECT_TASKMENU
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENU�̏�����
 *
 *	@param	menu_res	���\�[�X
 */
//-----------------------------------------------------------------------------
static APP_TASKMENU_WORK * UNSelect_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *msg, HEAPID heapID )
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
#endif //UN_SELECT_TASKMENU


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
static UN_SELECT_MAIN_WORK* proc_init( GFL_PROC* proc, UN_SELECT_PARAM* prm )
{
  UN_SELECT_MAIN_WORK* wk;

	//�I�[�o�[���C�ǂݍ���
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	
	//�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_UN_SELECT, UN_SELECT_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(UN_SELECT_MAIN_WORK), HEAPID_UN_SELECT );
  GFL_STD_MemClear( wk, sizeof(UN_SELECT_MAIN_WORK) );

  // ������
  wk->heapID = HEAPID_UN_SELECT;
	
	//�`��ݒ菉����
	wk->graphic	= UN_SELECT_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heapID );

	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	//���b�Z�[�W
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
			NARC_message_mictest_dat, wk->heapID );

	//PRINT_QUE�쐬
	wk->print_que		= PRINTSYS_QUE_Create( wk->heapID );

	//BG���\�[�X�ǂݍ���
	UNSelect_BG_LoadResource( &wk->wk_bg, wk->heapID );

#ifdef UN_SELECT_TOUCHBAR
	//�^�b�`�o�[�̏�����
	{	
		GFL_CLUNIT	*clunit	= UN_SELECT_GRAPHIC_GetClunit( wk->graphic );
		wk->touchbar	= UNSelect_TOUCHBAR_Init( clunit, wk->heapID );
	}
#endif //UN_SELECT_TOUCHBAR

#ifdef UN_SELECT_TASKMENU
	//TASKMENU���\�[�X�ǂݍ��݁�������
	wk->menu_res	= APP_TASKMENU_RES_Create( BG_FRAME_BAR_M, PLTID_BG_TASKMENU_M, wk->font, wk->print_que, wk->heapID );
	wk->menu			= UNSelect_TASKMENU_Init( wk->menu_res, wk->msg, wk->heapID );
#endif //UN_SELECT_TASKMENU

  return wk;
}

