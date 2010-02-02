//=============================================================================
/**
 *
 *	@file		beacon_detail.c
 *	@brief  ����Ⴂ�ڍ׉��
 *	@author	Miyuki Iwasawa
 *	@data		2010.02.01
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
#include "beacon_detail_gra.h"

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
#include "app/beacon_detail.h"


//@TODO BG�ǂݍ��� �Ƃ肠�����}�C�N�e�X�g�̃��\�[�X
#include "message.naix"
#include "mictest.naix"	// �A�[�J�C�u
#include "msg/msg_mictest.h"  // GMM
#include "townmap_gra.naix"		// �^�b�`�o�[�J�X�^���{�^���p�T���v���Ƀ^�E���}�b�v���\�[�X
#include "beacon_status.naix"		// �^�b�`�o�[�J�X�^���{�^���p�T���v���Ƀ^�E���}�b�v���\�[�X

#include "beacon_detail_def.h"

//=============================================================================
// ���Ldefine���R�����g�A�E�g����ƁA�@�\����菜���܂�
//=============================================================================
#define BEACON_DETAIL_PMSDRAW       // �ȈՉ�b�\��

FS_EXTERN_OVERLAY(ui_common);

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
///	BG�Ǘ����[�N
//==============================================================
typedef struct 
{
	int dummy;
  ARCHANDLE* handle;
} BEACON_DETAIL_BG_WORK;


#ifdef BEACON_DETAIL_PMSDRAW
#define BEACON_DETAIL_PMSDRAW_NUM (3) ///< �ȈՉ�b�̌�
#endif // BEACON_DETAIL_PMSDRAW 

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heapID;
  int                       seq;
	int											  sub_seq;

	BEACON_DETAIL_BG_WORK				wk_bg;

	//�`��ݒ�
	BEACON_DETAIL_GRAPHIC_WORK	*graphic;
  ARCHANDLE* handle;

	//�^�b�`�o�[
	TOUCHBAR_WORK							*touchbar;

	//�t�H���g
	GFL_FONT									*font;

	//�v�����g�L���[
	PRINT_QUE									*print_que;
	GFL_MSGDATA								*msg;

#ifdef	BEACON_DETAIL_PRINT_TOOL
	//�v�����g���[�e�B���e�B
	PRINT_UTIL								print_util;
#endif	//BEACON_DETAIL_PRINT_TOOL

#ifdef BEACON_DETAIL_PMSDRAW
  GFL_BMPWIN*               pms_win[ BEACON_DETAIL_PMSDRAW_NUM ];
  PMS_DRAW_WORK*            pms_draw;
#endif //BEACON_DETAIL_PMSDRAW

} BEACON_DETAIL_WORK;


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
static GFL_PROC_RESULT BeaconDetailProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT BeaconDetailProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT BeaconDetailProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );


static int seq_Main( BEACON_DETAIL_WORK* wk );
static int seq_FadeIn( BEACON_DETAIL_WORK* wk );
static int seq_FadeOut( BEACON_DETAIL_WORK* wk );

//-------------------------------------
///	�ėp�������[�e�B���e�B
//=====================================
static void BeaconDetail_BGResInit( BEACON_DETAIL_WORK* wk, HEAPID heapID );
static void BeaconDetail_BGResRelease( BEACON_DETAIL_WORK* wk );

//-------------------------------------
///	�^�b�`�o�[
//=====================================
static TOUCHBAR_WORK * BeaconDetail_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heapID );
static void BeaconDetail_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar );
static void BeaconDetail_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar );

#ifdef BEACON_DETAIL_PMSDRAW
//-------------------------------------
///	�ȈՉ�b�\��
//=====================================
static void BeaconDetail_PMSDRAW_Init( BEACON_DETAIL_WORK* wk );
static void BeaconDetail_PMSDRAW_Exit( BEACON_DETAIL_WORK* wk );
static void BeaconDetail_PMSDRAW_Proc( BEACON_DETAIL_WORK* wk );
#endif // BEACON_DETAIL_PMSDRAW


//=============================================================================
/**
 *								�O�����J
 */
//=============================================================================
const GFL_PROC_DATA BeaconDetailProcData = 
{
	BeaconDetailProc_Init,
	BeaconDetailProc_Main,
	BeaconDetailProc_Exit,
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
 *	@param	*pwk					BEACON_DETAIL_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BeaconDetailProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
	BEACON_DETAIL_WORK *wk;
	BEACON_DETAIL_PARAM *param;

	//�I�[�o�[���C�ǂݍ���
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	
	//�����擾
	param	= pwk;

	//�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BEACON_DETAIL, BEACON_DETAIL_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(BEACON_DETAIL_WORK), HEAPID_BEACON_DETAIL );
  GFL_STD_MemClear( wk, sizeof(BEACON_DETAIL_WORK) );

  // ������
  wk->heapID = HEAPID_BEACON_DETAIL;
	
	//�`��ݒ菉����
	wk->graphic	= BEACON_DETAIL_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heapID );

	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	//���b�Z�[�W
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
			NARC_message_mictest_dat, wk->heapID );

	//PRINT_QUE�쐬
	wk->print_que		= PRINTSYS_QUE_Create( wk->heapID );

	//BG���\�[�X�ǂݍ���
	BeaconDetail_BGResInit( wk, wk->heapID );

	//�^�b�`�o�[�̏�����
	{	
		GFL_CLUNIT	*clunit	= BEACON_DETAIL_GRAPHIC_GetClunit( wk->graphic );
		wk->touchbar	= BeaconDetail_TOUCHBAR_Init( clunit, wk->heapID );
	}

#ifdef BEACON_DETAIL_PMSDRAW  
  BeaconDetail_PMSDRAW_Init( wk );
#endif //BEACON_DETAIL_PMSDRAW

#if 0
	//@todo	�t�F�[�h�V�[�P���X���Ȃ��̂�
	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(0);
#endif

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  PROC �I������
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					BEACON_DETAIL_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BeaconDetailProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	BEACON_DETAIL_WORK* wk = mywk;

	//�^�b�`�o�[
	BeaconDetail_TOUCHBAR_Exit( wk->touchbar );

#ifdef BEACON_DETAIL_PMSDRAW
  BeaconDetail_PMSDRAW_Exit( wk );
#endif //BEACON_DETAIL_PMSDRAW
	
  //BG���\�[�X�j��
	BeaconDetail_BGResRelease( wk );

	//���b�Z�[�W�j��
	GFL_MSG_Delete( wk->msg );

	//PRINT_QUE
	PRINTSYS_QUE_Delete( wk->print_que );

	//FONT
	GFL_FONT_Delete( wk->font );

	//�`��ݒ�j��
	BEACON_DETAIL_GRAPHIC_Exit( wk->graphic );

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
 *	@param	*pwk					BEACON_DETAIL_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BeaconDetailProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	BEACON_DETAIL_WORK* wk = mywk;

  switch(*seq){
  case SEQ_FADEIN:
    *seq = seq_FadeIn( wk );
    break;
  case SEQ_MAIN:
    *seq = seq_Main( wk );
    break;
  case SEQ_FADEOUT:
    *seq = seq_FadeOut( wk );
    break;
  case SEQ_EXIT:
    return GFL_PROC_RES_FINISH;
  }

	//PRINT_QUE
	PRINTSYS_QUE_Main( wk->print_que );

#ifdef BEACON_DETAIL_PMSDRAW
  BeaconDetail_PMSDRAW_Proc( wk );
#endif //BEACON_DETAIL_PMSDRAW

	//2D�`��
	BEACON_DETAIL_GRAPHIC_2D_Draw( wk->graphic );

  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

/*
 *  @brief  ���C��
 */
static int seq_Main( BEACON_DETAIL_WORK* wk )
{
  // �f�o�b�O�{�^���ŃA�v���I��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
  {
    return SEQ_FADEOUT;
  }
	
  //�^�b�`�o�[���C������
	BeaconDetail_TOUCHBAR_Main( wk->touchbar );

  return SEQ_MAIN;
}

/*
 *  @brief  �t�F�[�h�C��
 */
static int seq_FadeIn( BEACON_DETAIL_WORK* wk )
{
  switch( wk->seq ){
  case 0:
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
    wk->seq++;
    break;
  case 1:
    if( !GFL_FADE_CheckFade() ){	
      wk->seq = 0;
			return SEQ_MAIN;
		}
    break;
  }
  return SEQ_FADEIN; 
}

/*
 *  @brief  �t�F�[�h�A�E�g
 */
static int seq_FadeOut( BEACON_DETAIL_WORK* wk )
{
  switch( wk->seq ){
  case 0:
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    wk->seq++;
    break;
  case 1:
    if( !GFL_FADE_CheckFade() ){	
      wk->seq = 0;
      return SEQ_EXIT; 
		}
    break;
  }
  return SEQ_FADEOUT; 
}

//-----------------------------------------------------------------------------
/**
 *	@brief  BG�Ǘ����W���[�� ���\�[�X�ǂݍ���
 *
 *	@param	BEACON_DETAIL_BG_WORK* wk BG�Ǘ����[�N
 *	@param	heapID  �q�[�vID 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void BeaconDetail_BGResInit( BEACON_DETAIL_WORK* wk, HEAPID heapID )
{
  ARCHANDLE* tmap_h;

	wk->handle	= GFL_ARC_OpenDataHandle( ARCID_BEACON_STATUS, heapID );
	tmap_h	= GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, heapID );

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette( tmap_h, NARC_townmap_gra_tmap_bg_d_NCLR, PALTYPE_MAIN_BG, 0, 0x200, heapID );
	GFL_ARCHDL_UTIL_TransVramPalette( wk->handle, NARC_beacon_status_bdetail_bgu_nclr, PALTYPE_SUB_BG, 0, 0x200, heapID );

	//	----- ���� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	wk->handle, NARC_beacon_status_bdetail_bgu_lz_ncgr,
						BG_FRAME_WIN01_S, 0, 0, TRUE, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	wk->handle, NARC_beacon_status_bdetail_bgu01_lz_nscr,
						BG_FRAME_WIN01_S, 0, 0, TRUE, heapID );		
	GFL_ARCHDL_UTIL_TransVramScreen(	wk->handle, NARC_beacon_status_bdetail_bgu01_lz_nscr,
						BG_FRAME_WIN02_S, 0, 0, TRUE, heapID );		
	
  //	----- ����� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	tmap_h, NARC_townmap_gra_tmap_bg_d_NCGR,
						BG_FRAME_MAP01_M, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	tmap_h, NARC_townmap_gra_tmap_root_d_NSCR,
						BG_FRAME_MAP01_M, 0, 0x800, 0, heapID );	
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	tmap_h, NARC_townmap_gra_tmap_map_d_NCGR,
						BG_FRAME_MAP02_M, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	tmap_h, NARC_townmap_gra_tmap_map_d_NSCR,
						BG_FRAME_MAP02_M, 0, 0x800, 0, heapID );	

  GFL_ARC_CloseDataHandle( tmap_h );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  BG�Ǘ����W���[�� ���\�[�X���
 *
 *	@param	BEACON_DETAIL_WORK* wk �Ǘ����[�N
 *	@param	heapID  �q�[�vID 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void BeaconDetail_BGResRelease( BEACON_DETAIL_WORK* wk )
{
	GFL_ARC_CloseDataHandle( wk->handle );
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
 *	@param	heapID							�q�[�vID
 *
 *	@return	TOUCHBAR_WORK
 */
//-----------------------------------------------------------------------------
static TOUCHBAR_WORK * BeaconDetail_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heapID )
{	
	//�A�C�R���̐ݒ�
	//�������
	static const TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]	=
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
	    TOUCHBAR_ICON_CUR_D,	//���{�^��
			{	TOUCHBAR_ICON_X_05, TOUCHBAR_ICON_Y },
		},
		{	
	    TOUCHBAR_ICON_CUR_U,	//���{�^��
			{	TOUCHBAR_ICON_X_04, TOUCHBAR_ICON_Y },
		},
	};

	//�ݒ�\����
	//�����قǂ̑����{���\�[�X���������
	TOUCHBAR_SETUP	touchbar_setup;
	GFL_STD_MemClear( &touchbar_setup, sizeof(TOUCHBAR_SETUP) );

	touchbar_setup.p_item		= (TOUCHBAR_ITEM_ICON*)touchbar_icon_tbl;				//��̑����
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
static void BeaconDetail_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar )
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
static void BeaconDetail_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Main( touchbar );
}

#ifdef	BEACON_DETAIL_PRINT_TOOL
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
static BOOL PrintTool_MainFunc( BEACON_DETAIL_WORK * wk )
{
	switch( wk->sub_seq ){
	case 0:
		PrintTool_AddBmpWin( wk );			// BMPWIN�쐬
		PrintTool_PrintHP( wk );				// �g�o�\��
		PrintTool_ScreenTrans( wk );		// �X�N���[���]��
		wk->sub_seq = 1;
		break;

	case 1:
		// �v�����g�I���҂�
		if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
			wk->sub_seq = 2;
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
static void PrintTool_AddBmpWin( BEACON_DETAIL_WORK * wk )
{
	wk->print_util.win = GFL_BMPWIN_Create(
													GFL_BG_FRAME0_M,					// �a�f�t���[��
													1, 1,											// �\�����W
													20, 2,										// �\���T�C�Y
													15,												// �p���b�g
													GFL_BMP_CHRAREA_GET_B );	// �L�����擾����
}

// BMPWIN�폜
static void PrintTool_DelBmpWin( BEACON_DETAIL_WORK * wk )
{
		GFL_BMPWIN_Delete( wk->print_util.win );
}

// BMPWIN�X�N���[���]��
static void PrintTool_ScreenTrans( BEACON_DETAIL_WORK * wk )
{
	GFL_BMPWIN_MakeScreen( wk->print_util.win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(wk->print_util.win) );
}

// �g�o�\��
static void PrintTool_PrintHP( BEACON_DETAIL_WORK * wk )
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
#endif	//BEACON_DETAIL_PRINT_TOOL

#ifdef BEACON_DETAIL_PMSDRAW

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\�� ����������
 *
 *	@param	BEACON_DETAIL_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void BeaconDetail_PMSDRAW_Init( BEACON_DETAIL_WORK* wk )
{
	GFL_CLUNIT	*clunit;
  
  clunit = BEACON_DETAIL_GRAPHIC_GetClunit( wk->graphic );

  wk->pms_draw  = PMS_DRAW_Init( clunit, CLSYS_DRAW_SUB, wk->print_que, wk->font, 
      PLTID_OBJ_PMS_DRAW, BEACON_DETAIL_PMSDRAW_NUM ,wk->heapID );
  
  {
    int i;
    PMS_DATA pms;

    // PMS�\���pBMPWIN����
    for( i=0; i<BEACON_DETAIL_PMSDRAW_NUM; i++ )
    {
      wk->pms_win[i] = GFL_BMPWIN_Create(
          BG_FRAME_DAT01_S,					// �a�f�t���[��
          2+4*i, 0 + 6 * i,					  	// �\�����W(�L�����P��)
          28, 4,    							  // �\���T�C�Y
          15,												// �p���b�g
          GFL_BMP_CHRAREA_GET_B );	// �L�����擾����
    }
    
    // 1�� �ʏ�+�󗓕\��
    PMSDAT_SetDebug( &pms );
    PMSDAT_SetWord( &pms, 1, PMS_WORD_NULL );
    PMS_DRAW_Print( wk->pms_draw, wk->pms_win[0], &pms ,0 );

    // 2�� �f�R��
    PMSDAT_SetDeco( &pms, 1, PMS_DECOID_HERO );
    PMS_DRAW_Print( wk->pms_draw, wk->pms_win[1], &pms ,1 );
    
    // 3�� �f�R����\��
    PMSDAT_SetDeco( &pms, 0, PMS_DECOID_TANKS );
    PMSDAT_SetDeco( &pms, 1, PMS_DECOID_LOVE );
    PMS_DRAW_Print( wk->pms_draw, wk->pms_win[2], &pms ,2 );
    
    // 1�̗v�f��2�ɃR�s�[(�ړ��\���Ȃǂɂ��g�p���������B)
    PMS_DRAW_Copy( wk->pms_draw, 1, 2 );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\�� �㏈��
 *
 *	@param	BEACON_DETAIL_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void BeaconDetail_PMSDRAW_Exit( BEACON_DETAIL_WORK* wk )
{
  PMS_DRAW_Exit( wk->pms_draw );
  {
    int i;
    for( i=0; i<BEACON_DETAIL_PMSDRAW_NUM; i++ )
    {
      GFL_BMPWIN_Delete( wk->pms_win[i] );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ȈՉ�b�\�� �又��
 *
 *	@param	BEACON_DETAIL_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void BeaconDetail_PMSDRAW_Proc( BEACON_DETAIL_WORK* wk )
{
#if 1
  // SELECT�ŃN���A
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    int i;
    for( i=0; i<BEACON_DETAIL_PMSDRAW_NUM; i++ )
    {
      PMS_DRAW_Clear( wk->pms_draw, i, TRUE );
    }
  }
  // START�Ń����_���}���i��d�o�^����ƃA�T�[�g�j
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
  {
    int i;
    for( i=0; i<BEACON_DETAIL_PMSDRAW_NUM; i++ )
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

#endif // BEACON_DETAIL_PMSDRAW

