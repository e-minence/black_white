//============================================================================================
/**
 * @file		zknsearch_seq.c
 * @brief		�}�ӌ������ �V�[�P���X����
 * @author	Hiroyuki Nakamura
 * @date		10.02.09
 *
 *	���W���[�����FZKNSEARCHSEQ
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "system/wipe.h"
#include "sound/pm_sndsys.h"
#include "ui/touchbar.h"

#include "../zukan_common.h"
#include "zknsearch_main.h"
#include "zknsearch_seq.h"
#include "zknsearch_obj.h"
#include "zknsearch_bmp.h"
#include "zknsearch_ui.h"
#include "zknsearch_list.h"
#include "zknsearch_snd_def.h"


//============================================================================================
//	�萔��`
//============================================================================================

// ���C���V�[�P���X
enum {
	MAINSEQ_INIT = 0,
	MAINSEQ_RELEASE,
	MAINSEQ_WIPE,

	MAINSEQ_INIT_MENU,
	MAINSEQ_MAIN_MENU,
	MAINSEQ_EXIT_MENU,

	MAINSEQ_RESET_SORT,
	MAINSEQ_START_SORT,
	MAINSEQ_RESULT_SORT,

	MAINSEQ_INIT_ROW,
	MAINSEQ_MAIN_ROW,
	MAINSEQ_EXIT_ROW,

	MAINSEQ_INIT_NAME,
	MAINSEQ_MAIN_NAME,
	MAINSEQ_EXIT_NAME,

	MAINSEQ_INIT_TYPE,
	MAINSEQ_MAIN_TYPE,
	MAINSEQ_EXIT_TYPE,

	MAINSEQ_INIT_COLOR,
	MAINSEQ_MAIN_COLOR,
	MAINSEQ_EXIT_COLOR,

	MAINSEQ_INIT_FORM,
	MAINSEQ_MAIN_FORM,
	MAINSEQ_EXIT_FORM,

	MAINSEQ_PAGE_MOVE,
	MAINSEQ_BUTTON_ANM,

	MAINSEQ_END_SET,
	MAINSEQ_END,
};

// �{�^���A�j���̎��
enum {
	BUTTON_ANM_BG = 0,
	BUTTON_ANM_OBJ
};

#define	SORT_COUNT_PUT_WINDOW		( 0 )					// �������J�E���g�F�E�B���h�E�\��
#define	SORT_COUNT_MAKE_ENGINE	( 1 )					// �������J�E���g�F���������G���W���쐬
#define	SORT_COUNT_SEARCH				( 2 )					// �������J�E���g�F�����J�n
#define	SORT_COUNT_STOP_SE			( 100 )				// �������J�E���g�F�������r�d��~
#define	SORT_COUNT_SEARCH_END		( 60*2+1 )		// �������J�E���g�F�����I��


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static int MainSeq_Init( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_Release( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_Wipe( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_InitMenu( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_MainMenu( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_ExitMenu( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_ResetSort( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_StartSort( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_ResultSort( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_InitRow( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_MainRow( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_ExitRow( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_InitName( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_MainName( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_ExitName( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_InitType( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_MainType( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_ExitType( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_InitColor( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_MainColor( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_ExitColor( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_InitForm( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_MainForm( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_ExitForm( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_PageMove( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_ButtonAnm( ZKNSEARCHMAIN_WORK * wk );
static int MainSeq_EndSet( ZKNSEARCHMAIN_WORK * wk );

static void ChangeSortRow( ZKNSEARCHMAIN_WORK * wk, u32 pos );
static void ResetSortRow( ZKNSEARCHMAIN_WORK * wk );
static void ChangeSortName( ZKNSEARCHMAIN_WORK * wk, u32 pos );
static void ResetSortName( ZKNSEARCHMAIN_WORK * wk );
static void ChangeSortType( ZKNSEARCHMAIN_WORK * wk, u32 pos );
static void ResetSortType( ZKNSEARCHMAIN_WORK * wk );
static void ChangeSortColor( ZKNSEARCHMAIN_WORK * wk, u32 pos );
static void ResetSortColor( ZKNSEARCHMAIN_WORK * wk );
static void ChangeSortForm( ZKNSEARCHMAIN_WORK * wk, u32 pos );
static void ResetSortForm( ZKNSEARCHMAIN_WORK * wk );

static int SetWipeIn( ZKNSEARCHMAIN_WORK * wk, int next );
static int SetWipeOut( ZKNSEARCHMAIN_WORK * wk, int next );

static int SetButtonAnm( ZKNSEARCHMAIN_WORK * wk, u8 mode, u8 id, int next );
static int PageChange( ZKNSEARCHMAIN_WORK * wk, int next );

static void SetShortCut( ZKNSEARCHMAIN_WORK * wk );


FS_EXTERN_OVERLAY(ui_common);


//============================================================================================
//	�O���[�o��
//============================================================================================

// ���C���V�[�P���X
static const pZKNSEARCH_FUNC MainSeq[] = {
	MainSeq_Init,
	MainSeq_Release,
	MainSeq_Wipe,

	MainSeq_InitMenu,
	MainSeq_MainMenu,
	MainSeq_ExitMenu,

	MainSeq_ResetSort,
	MainSeq_StartSort,
	MainSeq_ResultSort,

	MainSeq_InitRow,
	MainSeq_MainRow,
	MainSeq_ExitRow,

	MainSeq_InitName,
	MainSeq_MainName,
	MainSeq_ExitName,

	MainSeq_InitType,
	MainSeq_MainType,
	MainSeq_ExitType,

	MainSeq_InitColor,
	MainSeq_MainColor,
	MainSeq_ExitColor,

	MainSeq_InitForm,
	MainSeq_MainForm,
	MainSeq_ExitForm,

	MainSeq_PageMove,
	MainSeq_ButtonAnm,
	MainSeq_EndSet,
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @retval	"TRUE = ������"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZKNSEARCHSEQ_MainSeq( ZKNSEARCHMAIN_WORK * wk )
{
	wk->mainSeq = MainSeq[wk->mainSeq]( wk );
	if( wk->mainSeq == MAINSEQ_END ){
		return FALSE;
	}

	ZKNSEARCHOBJ_AnmMain( wk );
	ZKNSEARCHBMP_PrintUtilTrans( wk );
	ZKNCOMM_ScrollBaseBG( GFL_BG_FRAME2_M, GFL_BG_FRAME2_S, &wk->BaseScroll );

	return TRUE;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F������
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Init( ZKNSEARCHMAIN_WORK * wk )
{
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �T�u��ʂ����C����
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );
	// �P�x���Œ�ɂ��Ă���
	GX_SetMasterBrightness( -16 );
	GXS_SetMasterBrightness( -16 );

	// �S�����Z�b�g����̂ł͂Ȃ��A���[�h�����ύX���� 
	if( ZUKANSAVE_GetZukanMode( wk->dat->savedata ) == TRUE ){ 
		wk->dat->sort->mode = ZKNCOMM_LIST_SORT_MODE_ZENKOKU;
	}else{
		wk->dat->sort->mode = ZKNCOMM_LIST_SORT_MODE_LOCAL;
	}

	ZKNSEARCHMAIN_InitVram();
	ZKNSEARCHMAIN_InitBg();
	ZKNSEARCHMAIN_LoadBgGraphic();
	ZKNSEARCHMAIN_InitMsg( wk );

	ZKNSEARCHBMP_Init( wk );
	ZKNSEARCHOBJ_Init( wk );

	ZKNSEARCHMAIN_InitBlinkAnm( wk );

	GFL_NET_ReloadIconTopOrBottom( TRUE, HEAPID_ZUKAN_SEARCH );

	ZKNSEARCHMAIN_InitBgWinFrame( wk );
	ZKNSEARCHMAIN_InitFrameScroll( wk );

	ZKNSEARCHMAIN_InitPaletteFade( wk );
	ZKNSEARCHMAIN_SetPalFadeSeq( wk, 16, 16 );

	ZKNSEARCHMAIN_LoadLoadingWindow( wk );

	ZKNSEARCHMAIN_SetWindow();

	ZKNSEARCHMAIN_InitVBlank( wk );
	ZKNSEARCHMAIN_InitHBlank( wk );

	wk->page = 0xff;
  
	ZKNSEARCHMAIN_ConvFormDataToList( wk );		// �f�[�^���烊�X�g��

	return MAINSEQ_INIT_MENU;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Release( ZKNSEARCHMAIN_WORK * wk )
{
	if( ZKNSEARCHMAIN_MainSrameScroll(wk) == TRUE || PaletteFadeCheck(wk->pfd) != 0 ){
		return MAINSEQ_RELEASE;
	}

	ZKNSEARCHMAIN_ExitHBlank( wk );
	ZKNSEARCHMAIN_ExitVBlank( wk );

	ZKNSEARCHMAIN_ResetWindow();

	ZKNSEARCHMAIN_UnloadLoadingWindow( wk );

	ZKNSEARCHMAIN_ExitBgWinFrame( wk );

	ZKNSEARCHMAIN_ExitPaletteFade( wk );

	ZKNSEARCHMAIN_ExitBlinkAnm( wk );

	ZKNSEARCHOBJ_Exit( wk );
	ZKNSEARCHBMP_Exit( wk );

	ZKNSEARCHMAIN_ExitMsg( wk );
	ZKNSEARCHMAIN_ExitBg();

	// �P�x���Œ�ɂ��Ă���
	GX_SetMasterBrightness( -16 );
	GXS_SetMasterBrightness( -16 );
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );

	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

	return MAINSEQ_END;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�t�F�[�h�҂�
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Wipe( ZKNSEARCHMAIN_WORK * wk )
{
	if( PaletteFadeCheck(wk->pfd) == 0 ){
		return wk->funcSeq;
	}
	return MAINSEQ_WIPE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���C����ʏ�����
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_InitMenu( ZKNSEARCHMAIN_WORK * wk )
{
	if( PaletteFadeCheck(wk->pfd) != 0 ){
		return MAINSEQ_INIT_MENU;
	}

	ZKNSEARCHMAIN_SetBlendAlpha( TRUE );

	ZKNSEARCHMAIN_LoadMenuPageScreen( wk );

	ZKNSEARCHBMP_PutMainPage( wk );
	ZKNSEARCHOBJ_PutMainPage( wk );

	ZKNSEARCHMAIN_ChangeBgPriorityMenu();
	ZKNSEARCHMAIN_ListBGOn( wk );

	if( wk->page == 0xff ){
		ZKNSEARCHUI_MainCursorMoveInit( wk, 0 );
		GX_SetMasterBrightness( 0 );
		GXS_SetMasterBrightness( 0 );
		ZKNSEARCHMAIN_SetFrameScrollParam( wk, -ZKNCOMM_BAR_SPEED );
		ZKNSEARCHMAIN_SetPalFadeSeq( wk, 16, 0 );
		return MAINSEQ_MAIN_MENU;
	}
	ZKNSEARCHUI_MainCursorMoveInit( wk, wk->page );
	return MAINSEQ_MAIN_MENU;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���C����ʃ��C��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MainMenu( ZKNSEARCHMAIN_WORK * wk )
{
	if( ZKNSEARCHMAIN_MainSrameScroll(wk) == TRUE || PaletteFadeCheck(wk->pfd) != 0 ){
		return MAINSEQ_MAIN_MENU;
	}

	switch( ZKNSEARCHUI_MenuMain(wk) ){
	case ZKNSEARCHUI_ROW:
		PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
		return SetButtonAnm( wk, BUTTON_ANM_BG, 0, PageChange(wk,MAINSEQ_INIT_ROW) );

	case ZKNSEARCHUI_NAME:
		PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
		return SetButtonAnm( wk, BUTTON_ANM_BG, 1, PageChange(wk,MAINSEQ_INIT_NAME) );

	case ZKNSEARCHUI_TYPE:
		PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
		return SetButtonAnm( wk, BUTTON_ANM_BG, 2, PageChange(wk,MAINSEQ_INIT_TYPE) );

	case ZKNSEARCHUI_COLOR:
		PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
		return SetButtonAnm( wk, BUTTON_ANM_BG, 3, PageChange(wk,MAINSEQ_INIT_COLOR) );

	case ZKNSEARCHUI_FORM:
		PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
		return SetButtonAnm( wk, BUTTON_ANM_BG, 4, PageChange(wk,MAINSEQ_INIT_FORM) );

	case ZKNSEARCHUI_START:
		PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
		return SetButtonAnm( wk, BUTTON_ANM_BG, 5, MAINSEQ_START_SORT );

	case ZKNSEARCHUI_RESET:
		PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
		return SetButtonAnm( wk, BUTTON_ANM_BG, 6, MAINSEQ_RESET_SORT );

	case ZKNSEARCHUI_Y:
		PMSND_PlaySE( ZKNSEARCH_SE_Y );
		SetShortCut( wk );
		break;

	case ZKNSEARCHUI_X:
		PMSND_PlaySE( ZKNSEARCH_SE_CLOASE );
		wk->dat->retMode = ZKNSEARCH_RET_EXIT_X;
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_EXIT, APP_COMMON_BARICON_EXIT_ON );
		return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_EXIT, MAINSEQ_END_SET );

	case ZKNSEARCHUI_RET:
		PMSND_PlaySE( ZKNSEARCH_SE_CANCEL );
		ZKNSEARCHMAIN_ConvFormListToData( wk );		// ���X�g����f�[�^��
		wk->dat->retMode = ZKNSEARCH_RET_CANCEL;
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON );
		return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RETURN, MAINSEQ_END_SET );

	case CURSORMOVE_CURSOR_ON:		// �J�[�\���\��
		PMSND_PlaySE( ZKNSEARCH_SE_MOVE );
		break;

	case CURSORMOVE_CURSOR_MOVE:	// �ړ�
		PMSND_PlaySE( ZKNSEARCH_SE_MOVE );
		break;

	case CURSORMOVE_CANCEL:					// �L�����Z��
		PMSND_PlaySE( ZKNSEARCH_SE_CANCEL );
		ZKNSEARCHMAIN_ConvFormListToData( wk );		// ���X�g����f�[�^��
		wk->dat->retMode = ZKNSEARCH_RET_CANCEL;
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON );
		return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RETURN, MAINSEQ_END_SET );
	}

	BLINKPALANM_Main( wk->blink );

	return MAINSEQ_MAIN_MENU;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���C����ʏI��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ExitMenu( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHMAIN_ListBGOff( wk );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_LABEL, FALSE );
	ZKNSEARCHUI_CursorMoveExit( wk );

	return wk->nextSeq;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�����f�[�^���Z�b�g
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ResetSort( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNCOMM_ResetSortData( wk->dat->savedata, wk->dat->sort );
	ZKNSEARCHBMP_PutMainPage( wk );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_LABEL, FALSE );
	return MAINSEQ_MAIN_MENU;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�����J�n
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_StartSort( ZKNSEARCHMAIN_WORK * wk )
{
	// �E�B���h�E�\��
	if( wk->loadingCnt == SORT_COUNT_PUT_WINDOW ){
		PMSND_PlaySE( ZKNSEARCH_SE_LOADING );
		ZKNSEARCHMAIN_LoadingWindowOn( wk );
		ZKNSEARCHBMP_SearchStart( wk );
		ZKNSEARCHBMP_ClearMainPageLabel( wk );
	}

	// �������r�d��~
	if( wk->loadingCnt == SORT_COUNT_STOP_SE ){
		PMSND_StopSE();
	}

  // ���������G���W���쐬
	if( wk->loadingCnt == SORT_COUNT_MAKE_ENGINE ){
		ZKNSEARCHMAIN_ConvFormListToData( wk );		// ���X�g����f�[�^��
		wk->egn_wk = ZUKAN_SEARCH_ENGINE_DivInit(
									wk->dat->savedata, wk->dat->sort, HEAPID_ZUKAN_SEARCH_L );
		wk->egn_st = ZKN_SCH_EGN_DIV_STATE_CONTINUE;
	}

	// ��������
	if( wk->loadingCnt >= SORT_COUNT_SEARCH && wk->egn_st == ZKN_SCH_EGN_DIV_STATE_CONTINUE ){
		u16  num;
		u16 * list;
		// ����
		wk->egn_st = ZUKAN_SEARCH_ENGINE_DivSearch(
									wk->egn_wk, HEAPID_ZUKAN_SYS, &num, &list );
		if( wk->egn_st == ZKN_SCH_EGN_DIV_STATE_FINISH ){
			wk->dat->listMax = num;
			wk->dat->list = list;
			// �j��
			ZUKAN_SEARCH_ENGINE_DivExit( wk->egn_wk );
		}
	}

	// �����I��
	if( wk->loadingCnt == SORT_COUNT_SEARCH_END ){
		if( wk->egn_st == ZKN_SCH_EGN_DIV_STATE_FINISH ){
			if( wk->dat->listMax != 0 ){
				PMSND_PlaySE( ZKNSEARCH_SE_HIT );
				ZKNSEARCHBMP_SearchComp( wk );
			}else{
				PMSND_PlaySE( ZKNSEARCH_SE_ERROR );
				ZKNSEARCHBMP_SearchError( wk );
			}
			wk->loadingCnt = 0;
			return MAINSEQ_RESULT_SORT;
		}
	// ������
	}else{
		ZKNSEARCHOBJ_MoveLoadingBar( wk, wk->loadingCnt );
		wk->loadingCnt++;
	}

	return MAINSEQ_START_SORT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F��������
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ResultSort( ZKNSEARCHMAIN_WORK * wk )
{
	if( ZKNSEARCHUI_Result( wk ) == TRUE ){
		if( wk->dat->listMax != 0 ){
			wk->dat->retMode = ZKNSEARCH_RET_START;
			return MAINSEQ_END_SET;
		}
		ZKNSEARCHMAIN_LoadingWindowOff( wk );
		ZKNSEARCHBMP_PutMainPageLabel( wk );
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			CURSORMOVE_CursorOnOffSet( wk->cmwk, FALSE );
		}else{
			CURSORMOVE_CursorOnOffSet( wk->cmwk, TRUE );
		}
		ZKNSEARCHMAIN_ConvFormDataToList( wk );		// �f�[�^���烊�X�g�֖߂�
		return MAINSEQ_MAIN_MENU;
	}
	return MAINSEQ_RESULT_SORT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���у��X�g������
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_InitRow( ZKNSEARCHMAIN_WORK * wk )
{
	switch( wk->pageSeq ){
	case 0:
		wk->page = ZKNSEARCHMAIN_PAGE_ROW;
		ZKNSEARCHMAIN_SetBlendAlpha( TRUE );
		ZKNSEARCHBMP_PutRowPage( wk );
		ZKNSEARCHLIST_MakeRowList( wk );
		wk->pageSeq++;
	case 1:
		if( FRAMELIST_Init( wk->lwk ) == FALSE ){
			ZKNSEARCHOBJ_PutRowPage( wk );
			ZKNSEARCHMAIN_LoadRowListPageScreen( wk );
			ZKNSEARCHMAIN_ChangeBgPriorityList();
			ZKNSEARCHMAIN_ListBGOn( wk );
			wk->pageSeq = 0;
			return MAINSEQ_MAIN_ROW;
		}
	}
	return MAINSEQ_INIT_ROW;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���у��X�g���C��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MainRow( ZKNSEARCHMAIN_WORK * wk )
{
	u32	ret = FRAMELIST_Main( wk->lwk );

	switch( ret ){
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY ){
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ChangeSortRow( wk, ret );
		}
		break;

	case FRAMELIST_RET_NONE:
		ret = ZKNSEARCHUI_ListMain( wk );

		switch( ret ){
		case ZKNSEARCHUI_LIST_RET:
			PMSND_PlaySE( ZKNSEARCH_SE_CANCEL );
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON );
			return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RETURN, MAINSEQ_EXIT_ROW );

		case ZKNSEARCHUI_LIST_CANCEL:
			PMSND_PlaySE( ZKNSEARCH_SE_CANCEL );
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON );
			return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RETURN, MAINSEQ_EXIT_ROW );

		case ZKNSEARCHUI_LIST_RESET:
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ResetSortRow( wk );
			break;

		case ZKNSEARCHUI_LIST_ENTER1:
		case ZKNSEARCHUI_LIST_ENTER2:
		case ZKNSEARCHUI_LIST_ENTER3:
		case ZKNSEARCHUI_LIST_ENTER4:
		case ZKNSEARCHUI_LIST_ENTER5:
		case ZKNSEARCHUI_LIST_ENTER6:
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ChangeSortRow( wk, ret-ZKNSEARCHUI_LIST_ENTER1 );
			break;
		}
	}

	return MAINSEQ_MAIN_ROW;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���у��X�g�I��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ExitRow( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHMAIN_ListBGOff( wk );
	ZKNSEARCHOBJ_VanishList( wk );
	ZKNSEARCHLIST_FreeList( wk );

	return MAINSEQ_INIT_MENU;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���O���X�g������
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_InitName( ZKNSEARCHMAIN_WORK * wk )
{
	switch( wk->pageSeq ){
	case 0:
		wk->page = ZKNSEARCHMAIN_PAGE_NAME;
		ZKNSEARCHMAIN_SetBlendAlpha( TRUE );
		ZKNSEARCHBMP_PutNamePage( wk );
		ZKNSEARCHLIST_MakeNameList( wk );
		wk->pageSeq++;
	case 1:
		if( FRAMELIST_Init( wk->lwk ) == FALSE ){
			ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
			ZKNSEARCHOBJ_PutNamePage( wk );
			ZKNSEARCHMAIN_LoadNameListPageScreen( wk );
			ZKNSEARCHMAIN_ChangeBgPriorityList();
			ZKNSEARCHMAIN_ListBGOn( wk );
			wk->pageSeq = 0;
			return MAINSEQ_MAIN_NAME;
		}
	}
	return MAINSEQ_INIT_NAME;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���O���X�g���C��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MainName( ZKNSEARCHMAIN_WORK * wk )
{
	int	seq;
	u32	ret;

	seq = MAINSEQ_MAIN_NAME;
	ret = FRAMELIST_Main( wk->lwk );

	switch( ret ){
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY ){
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ChangeSortName( wk, ret );
		}
		break;

	case FRAMELIST_RET_CURSOR_ON:		// �J�[�\���\��
		break;

	case FRAMELIST_RET_MOVE:				// �J�[�\���ړ�
	case FRAMELIST_RET_SCROLL:			// �X�N���[���ʏ�
	case FRAMELIST_RET_RAIL:				// ���[���X�N���[��
	case FRAMELIST_RET_SLIDE:				// �X���C�h�X�N���[��
		ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
		break;

	case FRAMELIST_RET_PAGE_UP:			// �P�y�[�W���
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT_ON );
		wk->btnID = ZKNSEARCHOBJ_IDX_TB_LEFT;
		wk->subSeq = 0;
		wk->nextSeq = MAINSEQ_MAIN_NAME;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_PAGE_DOWN:		// �P�y�[�W����
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT_ON );
		wk->btnID = ZKNSEARCHOBJ_IDX_TB_RIGHT;
		wk->subSeq = 0;
		wk->nextSeq = MAINSEQ_MAIN_NAME;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_PAGE_UP_NONE:		// �y�[�W��������Ȃ��ĂP�y�[�W��ɃX�N���[���ł��Ȃ�����
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT_ON );
		return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_LEFT, MAINSEQ_MAIN_NAME );

	case FRAMELIST_RET_PAGE_DOWN_NONE:	// �y�[�W��������Ȃ��ĂP�y�[�W���ɃX�N���[���ł��Ȃ�����
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT_ON );
		return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RIGHT, MAINSEQ_MAIN_NAME );

	case FRAMELIST_RET_JUMP_TOP:		// ���X�g�ŏ㕔�փW�����v
	case FRAMELIST_RET_JUMP_BOTTOM:	// ���X�g�ŉ����փW�����v
		ZKNSEARCHOBJ_VanishMark( wk );
		ZKNSEARCHOBJ_VanishJumpMark( wk, 0, wk->dat->sort->name );
		wk->btnID = ZKNSEARCHOBJ_IDX_MAX;
		wk->subSeq = 0;
		wk->nextSeq = MAINSEQ_MAIN_NAME;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_NONE:
		ret = ZKNSEARCHUI_ListMain( wk );

		switch( ret ){
		case ZKNSEARCHUI_LIST_RET:
			PMSND_PlaySE( ZKNSEARCH_SE_CANCEL );
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON );
			return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RETURN, MAINSEQ_EXIT_NAME );

		case ZKNSEARCHUI_LIST_CANCEL:
			PMSND_PlaySE( ZKNSEARCH_SE_CANCEL );
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON );
			return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RETURN, MAINSEQ_EXIT_NAME );

		case ZKNSEARCHUI_LIST_RESET:
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ResetSortName( wk );
			break;

		case ZKNSEARCHUI_LIST_ENTER1:
		case ZKNSEARCHUI_LIST_ENTER2:
		case ZKNSEARCHUI_LIST_ENTER3:
		case ZKNSEARCHUI_LIST_ENTER4:
		case ZKNSEARCHUI_LIST_ENTER5:
		case ZKNSEARCHUI_LIST_ENTER6:
		case ZKNSEARCHUI_LIST_ENTER7:
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
			ChangeSortName( wk, ret-ZKNSEARCHUI_LIST_ENTER1 );
			break;
		}
	}

	return seq;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���O���X�g�I��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ExitName( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHMAIN_ListBGOff( wk );
	ZKNSEARCHOBJ_VanishList( wk );
	ZKNSEARCHLIST_FreeList( wk );

	return MAINSEQ_INIT_MENU;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�^�C�v���X�g������
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_InitType( ZKNSEARCHMAIN_WORK * wk )
{
	switch( wk->pageSeq ){
	case 0:
		wk->page = ZKNSEARCHMAIN_PAGE_TYPE;
		ZKNSEARCHMAIN_SetBlendAlpha( TRUE );
		ZKNSEARCHBMP_PutTypePage( wk );
		ZKNSEARCHLIST_MakeTypeList( wk );
		wk->pageSeq++;
	case 1:
		if( FRAMELIST_Init( wk->lwk ) == FALSE ){
			ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
			ZKNSEARCHOBJ_PutTypePage( wk );
			ZKNSEARCHMAIN_LoadTypeListPageScreen( wk );
			ZKNSEARCHMAIN_ChangeBgPriorityList();
			ZKNSEARCHMAIN_ListBGOn( wk );
			wk->pageSeq = 0;
			return MAINSEQ_MAIN_TYPE;
		}
	}
	return MAINSEQ_INIT_TYPE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�^�C�v���X�g���C��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MainType( ZKNSEARCHMAIN_WORK * wk )
{
	int	seq;
	u32	ret;

	seq = MAINSEQ_MAIN_TYPE;
	ret = FRAMELIST_Main( wk->lwk );

	switch( ret ){
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY ){
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ChangeSortType( wk, ret );
		}
		break;

	case FRAMELIST_RET_CURSOR_ON:		// �J�[�\���\��
		break;

	case FRAMELIST_RET_MOVE:				// �J�[�\���ړ�
	case FRAMELIST_RET_SCROLL:			// �X�N���[���ʏ�
	case FRAMELIST_RET_RAIL:				// ���[���X�N���[��
	case FRAMELIST_RET_SLIDE:				// �X���C�h�X�N���[��
		ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
		break;

	case FRAMELIST_RET_PAGE_UP:			// �P�y�[�W���
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT_ON );
		wk->btnID = ZKNSEARCHOBJ_IDX_TB_LEFT;
		wk->subSeq = 0;
		wk->nextSeq = MAINSEQ_MAIN_TYPE;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_PAGE_DOWN:		// �P�y�[�W����
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT_ON );
		wk->btnID = ZKNSEARCHOBJ_IDX_TB_RIGHT;
		wk->subSeq = 0;
		wk->nextSeq = MAINSEQ_MAIN_TYPE;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_PAGE_UP_NONE:		// �y�[�W��������Ȃ��ĂP�y�[�W��ɃX�N���[���ł��Ȃ�����
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT_ON );
		return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_LEFT, MAINSEQ_MAIN_TYPE );

	case FRAMELIST_RET_PAGE_DOWN_NONE:	// �y�[�W��������Ȃ��ĂP�y�[�W���ɃX�N���[���ł��Ȃ�����
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT_ON );
		return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RIGHT, MAINSEQ_MAIN_TYPE );

	case FRAMELIST_RET_JUMP_TOP:		// ���X�g�ŏ㕔�փW�����v
	case FRAMELIST_RET_JUMP_BOTTOM:	// ���X�g�ŉ����փW�����v
		ZKNSEARCHOBJ_VanishMark( wk );
		ZKNSEARCHOBJ_VanishJumpMark( wk, 0, ZKNSEARCHMAIN_GetSortTypeIndex(wk,wk->dat->sort->type1) );
		ZKNSEARCHOBJ_VanishJumpMark( wk, 1, ZKNSEARCHMAIN_GetSortTypeIndex(wk,wk->dat->sort->type2) );
		wk->btnID = ZKNSEARCHOBJ_IDX_MAX;
		wk->subSeq = 0;
		wk->nextSeq = MAINSEQ_MAIN_TYPE;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_NONE:
		ret = ZKNSEARCHUI_ListMain( wk );

		switch( ret ){
		case ZKNSEARCHUI_LIST_RET:
			PMSND_PlaySE( ZKNSEARCH_SE_CANCEL );
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON );
			return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RETURN, MAINSEQ_EXIT_TYPE );

		case ZKNSEARCHUI_LIST_CANCEL:
			PMSND_PlaySE( ZKNSEARCH_SE_CANCEL );
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON );
			return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RETURN, MAINSEQ_EXIT_TYPE );

		case ZKNSEARCHUI_LIST_RESET:
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ResetSortType( wk );
			break;

		case ZKNSEARCHUI_LIST_ENTER1:
		case ZKNSEARCHUI_LIST_ENTER2:
		case ZKNSEARCHUI_LIST_ENTER3:
		case ZKNSEARCHUI_LIST_ENTER4:
		case ZKNSEARCHUI_LIST_ENTER5:
		case ZKNSEARCHUI_LIST_ENTER6:
		case ZKNSEARCHUI_LIST_ENTER7:
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
			ChangeSortType( wk, ret-ZKNSEARCHUI_LIST_ENTER1 );
			break;
		}
	}

	return seq;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�^�C�v���X�g�I��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ExitType( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHMAIN_ListBGOff( wk );
	ZKNSEARCHOBJ_VanishList( wk );
	ZKNSEARCHLIST_FreeList( wk );

	return MAINSEQ_INIT_MENU;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�F���X�g������
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_InitColor( ZKNSEARCHMAIN_WORK * wk )
{
	switch( wk->pageSeq ){
	case 0:
		wk->page = ZKNSEARCHMAIN_PAGE_COLOR;
		ZKNSEARCHMAIN_SetBlendAlpha( TRUE );
		ZKNSEARCHBMP_PutColorPage( wk );
		ZKNSEARCHLIST_MakeColorList( wk );
		wk->pageSeq++;
	case 1:
		if( FRAMELIST_Init( wk->lwk ) == FALSE ){
			ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
			ZKNSEARCHOBJ_PutColorPage( wk );
			ZKNSEARCHMAIN_LoadColorListPageScreen( wk );
			ZKNSEARCHMAIN_ChangeBgPriorityList();
			ZKNSEARCHMAIN_ListBGOn( wk );
			wk->pageSeq = 0;
			return MAINSEQ_MAIN_COLOR;
		}
	}
	return MAINSEQ_INIT_COLOR;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�F���X�g���C��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MainColor( ZKNSEARCHMAIN_WORK * wk )
{
	int	seq;
	u32	ret;

	seq = MAINSEQ_MAIN_COLOR;
	ret = FRAMELIST_Main( wk->lwk );

	switch( ret ){
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY ){
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ChangeSortColor( wk, ret );
		}
		break;

	case FRAMELIST_RET_CURSOR_ON:		// �J�[�\���\��
		break;

	case FRAMELIST_RET_MOVE:				// �J�[�\���ړ�
	case FRAMELIST_RET_SCROLL:			// �X�N���[���ʏ�
	case FRAMELIST_RET_RAIL:				// ���[���X�N���[��
	case FRAMELIST_RET_SLIDE:				// �X���C�h�X�N���[��
		ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
		break;

	case FRAMELIST_RET_PAGE_UP:			// �P�y�[�W���
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT_ON );
		wk->btnID = ZKNSEARCHOBJ_IDX_TB_LEFT;
		wk->subSeq = 0;
		wk->nextSeq = MAINSEQ_MAIN_COLOR;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_PAGE_DOWN:		// �P�y�[�W����
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT_ON );
		wk->btnID = ZKNSEARCHOBJ_IDX_TB_RIGHT;
		wk->subSeq = 0;
		wk->nextSeq = MAINSEQ_MAIN_COLOR;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_PAGE_UP_NONE:		// �y�[�W��������Ȃ��ĂP�y�[�W��ɃX�N���[���ł��Ȃ�����
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT_ON );
		return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_LEFT, MAINSEQ_MAIN_COLOR );

	case FRAMELIST_RET_PAGE_DOWN_NONE:	// �y�[�W��������Ȃ��ĂP�y�[�W���ɃX�N���[���ł��Ȃ�����
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT_ON );
		return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RIGHT, MAINSEQ_MAIN_COLOR );

	case FRAMELIST_RET_JUMP_TOP:		// ���X�g�ŏ㕔�փW�����v
	case FRAMELIST_RET_JUMP_BOTTOM:	// ���X�g�ŉ����փW�����v
		ZKNSEARCHOBJ_VanishMark( wk );
		ZKNSEARCHOBJ_VanishJumpMark( wk, 0, wk->dat->sort->color );
		wk->btnID = ZKNSEARCHOBJ_IDX_MAX;
		wk->subSeq = 0;
		wk->nextSeq = MAINSEQ_MAIN_COLOR;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_NONE:
		ret = ZKNSEARCHUI_ListMain( wk );

		switch( ret ){
		case ZKNSEARCHUI_LIST_RET:
			PMSND_PlaySE( ZKNSEARCH_SE_CANCEL );
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON );
			return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RETURN, MAINSEQ_EXIT_COLOR );

		case ZKNSEARCHUI_LIST_CANCEL:
			PMSND_PlaySE( ZKNSEARCH_SE_CANCEL );
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON );
			return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RETURN, MAINSEQ_EXIT_COLOR );

		case ZKNSEARCHUI_LIST_RESET:
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ResetSortColor( wk );
			break;

		case ZKNSEARCHUI_LIST_ENTER1:
		case ZKNSEARCHUI_LIST_ENTER2:
		case ZKNSEARCHUI_LIST_ENTER3:
		case ZKNSEARCHUI_LIST_ENTER4:
		case ZKNSEARCHUI_LIST_ENTER5:
		case ZKNSEARCHUI_LIST_ENTER6:
		case ZKNSEARCHUI_LIST_ENTER7:
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
			ChangeSortColor( wk, ret-ZKNSEARCHUI_LIST_ENTER1 );
			break;
		}
	}

	return seq;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�F���X�g�I��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ExitColor( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHMAIN_ListBGOff( wk );
	ZKNSEARCHOBJ_VanishList( wk );
	ZKNSEARCHLIST_FreeList( wk );

	return MAINSEQ_INIT_MENU;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�t�H�������X�g������
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_InitForm( ZKNSEARCHMAIN_WORK * wk )
{
	switch( wk->pageSeq ){
	case 0:
		wk->page = ZKNSEARCHMAIN_PAGE_FORM;
		ZKNSEARCHMAIN_SetBlendAlpha( TRUE );
		ZKNSEARCHBMP_PutFormPage( wk );
		ZKNSEARCHLIST_MakeFormList( wk );
		wk->pageSeq++;
	case 1:
		if( FRAMELIST_Init( wk->lwk ) == FALSE ){
			ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
			ZKNSEARCHOBJ_PutFormPage( wk );
			ZKNSEARCHMAIN_LoadFormListPageScreen( wk );
			ZKNSEARCHMAIN_ChangeBgPriorityList();
			ZKNSEARCHMAIN_ListBGOn( wk );
			wk->pageSeq = 0;
			return MAINSEQ_MAIN_FORM;
		}
	}
	return MAINSEQ_INIT_FORM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�t�H�������X�g���C��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_MainForm( ZKNSEARCHMAIN_WORK * wk )
{
	int	seq;
	u32	ret;

	seq = MAINSEQ_MAIN_FORM;
	ret = FRAMELIST_Main( wk->lwk );

	switch( ret ){
	case 0:
	case 1:
	case 2:
	case 3:
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY ){
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ChangeSortForm( wk, ret );
		}
		break;

	case FRAMELIST_RET_CURSOR_ON:		// �J�[�\���\��
		break;

	case FRAMELIST_RET_MOVE:				// �J�[�\���ړ�
	case FRAMELIST_RET_SCROLL:			// �X�N���[���ʏ�
	case FRAMELIST_RET_RAIL:				// ���[���X�N���[��
	case FRAMELIST_RET_SLIDE:				// �X���C�h�X�N���[��
		ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
		break;

	case FRAMELIST_RET_PAGE_UP:			// �P�y�[�W���
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT_ON );
		wk->btnID = ZKNSEARCHOBJ_IDX_TB_LEFT;
		wk->subSeq = 0;
		wk->nextSeq = MAINSEQ_MAIN_FORM;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_PAGE_DOWN:		// �P�y�[�W����
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT_ON );
		wk->btnID = ZKNSEARCHOBJ_IDX_TB_RIGHT;
		wk->subSeq = 0;
		wk->nextSeq = MAINSEQ_MAIN_FORM;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_PAGE_UP_NONE:		// �y�[�W��������Ȃ��ĂP�y�[�W��ɃX�N���[���ł��Ȃ�����
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT_ON );
		return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_LEFT, MAINSEQ_MAIN_FORM );

	case FRAMELIST_RET_PAGE_DOWN_NONE:	// �y�[�W��������Ȃ��ĂP�y�[�W���ɃX�N���[���ł��Ȃ�����
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT_ON );
		return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RIGHT, MAINSEQ_MAIN_FORM );

	case FRAMELIST_RET_JUMP_TOP:		// ���X�g�ŏ㕔�փW�����v
	case FRAMELIST_RET_JUMP_BOTTOM:	// ���X�g�ŉ����փW�����v
		ZKNSEARCHOBJ_VanishJumpFormIcon( wk );
		ZKNSEARCHOBJ_VanishMark( wk );
		ZKNSEARCHOBJ_VanishJumpMarkForm( wk );
		wk->btnID = ZKNSEARCHOBJ_IDX_MAX;
		wk->subSeq = 0;
		wk->nextSeq = MAINSEQ_MAIN_FORM;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_NONE:
		ret = ZKNSEARCHUI_FormListMain( wk );

		switch( ret ){
		case ZKNSEARCHUI_LIST_RET:
			PMSND_PlaySE( ZKNSEARCH_SE_CANCEL );
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON );
			return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RETURN, MAINSEQ_EXIT_FORM );

		case ZKNSEARCHUI_LIST_CANCEL:
			PMSND_PlaySE( ZKNSEARCH_SE_CANCEL );
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON );
			return SetButtonAnm( wk, BUTTON_ANM_OBJ, ZKNSEARCHOBJ_IDX_TB_RETURN, MAINSEQ_EXIT_FORM );

		case ZKNSEARCHUI_LIST_RESET:
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ResetSortForm( wk );
			break;

		case ZKNSEARCHUI_LIST_ENTER1:
		case ZKNSEARCHUI_LIST_ENTER2:
		case ZKNSEARCHUI_LIST_ENTER3:
		case ZKNSEARCHUI_LIST_ENTER4:
			PMSND_PlaySE( ZKNSEARCH_SE_DECIDE );
			ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
			ChangeSortForm( wk, ret-ZKNSEARCHUI_LIST_ENTER1 );
			break;
		}
	}

	return seq;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�t�H�������X�g�I��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ExitForm( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHMAIN_ListBGOff( wk );
	ZKNSEARCHOBJ_VanishList( wk );
	ZKNSEARCHLIST_FreeList( wk );

	return MAINSEQ_INIT_MENU;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���X�g�y�[�W����
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PageMove( ZKNSEARCHMAIN_WORK * wk )
{
	switch( wk->subSeq ){
	case 0:
		if( FRAMELIST_Main( wk->lwk ) == FRAMELIST_RET_NONE ){
			wk->subSeq++;
		}
		break;

	case 1:
		if( wk->btnID == ZKNSEARCHOBJ_IDX_MAX || ZKNSEARCHOBJ_CheckAnm( wk, wk->btnID ) == FALSE ){
			wk->subSeq = 0;
			ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
			return wk->nextSeq;
		}
		break;
	}

	return MAINSEQ_PAGE_MOVE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�{�^���A�j���҂�
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ButtonAnm( ZKNSEARCHMAIN_WORK * wk )
{
	if( wk->btnMode == BUTTON_ANM_BG ){
		switch( wk->btnSeq ){
		case 0:
		case 2:
			if( wk->btnCnt == 0 ){
				ZKNSEARCHUI_ChangeCursorPalette( wk, wk->btnID, ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF );
				wk->btnCnt = 4;
				wk->btnSeq++;
			}else{
				wk->btnCnt--;
			}
			break;

		case 1:
		case 3:
			if( wk->btnCnt == 0 ){
				ZKNSEARCHUI_ChangeCursorPalette( wk, wk->btnID, ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR );
				wk->btnCnt = 4;
				wk->btnSeq++;
			}else{
				wk->btnCnt--;
			}
			break;

		case 4:
			if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
				ZKNSEARCHUI_ChangeCursorPalette( wk, wk->btnID, ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF );
			}else{
				ZKNSEARCHUI_ChangeCursorPalette( wk, wk->btnID, ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR2 );
			}
			return wk->funcSeq;
		}
	}else{
		if( ZKNSEARCHOBJ_CheckAnm( wk, wk->btnID ) == FALSE ){
			ZKNSEARCHOBJ_SetListPageArrowAnime( wk, FALSE );
			return wk->funcSeq;
		}
	}

	return MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�I���ݒ�
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_EndSet( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHUI_CursorMoveExit( wk );
	ZKNSEARCHMAIN_SetFrameScrollParam( wk, ZKNCOMM_BAR_SPEED );
	ZKNSEARCHMAIN_SetPalFadeSeq( wk, 0, 16 );
	return MAINSEQ_RELEASE;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�[�^�؂�ւ��F����
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		pos			�J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ChangeSortRow( ZKNSEARCHMAIN_WORK * wk, u32 pos )
{
	wk->dat->sort->row = pos;
	FRAMELIST_SetCursorPos( wk->lwk, pos );
	ZKNSEARCHBMP_PutRowItem( wk );
	ZKNSEARCHOBJ_ChangeMark( wk, pos, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�[�^���Z�b�g�F����
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ResetSortRow( ZKNSEARCHMAIN_WORK * wk )
{
	wk->dat->sort->row = ZKNCOMM_LIST_SORT_ROW_NUMBER;
	ZKNSEARCHBMP_PutRowItem( wk );
	ZKNSEARCHOBJ_ChangeMark( wk, 0, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�[�^�؂�ւ��F���O
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		pos			�J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ChangeSortName( ZKNSEARCHMAIN_WORK * wk, u32 pos )
{
	u32	list_pos = FRAMELIST_GetScrollCount( wk->lwk ) + pos;

	if( wk->dat->sort->name == list_pos ){
		wk->dat->sort->name = ZKNCOMM_LIST_SORT_NONE;
		ZKNSEARCHOBJ_ChangeMark( wk, pos, FALSE );
	}else{
		wk->dat->sort->name = list_pos;
		ZKNSEARCHOBJ_ChangeMark( wk, pos, TRUE );
	}
	FRAMELIST_SetCursorPos( wk->lwk, pos );
	ZKNSEARCHBMP_PutNameItem( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�[�^���Z�b�g�F���O
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ResetSortName( ZKNSEARCHMAIN_WORK * wk )
{
	wk->dat->sort->name = ZKNCOMM_LIST_SORT_NONE;
	ZKNSEARCHBMP_PutNameItem( wk );
	ZKNSEARCHOBJ_VanishMark( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�[�^�؂�ւ��F�^�C�v
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		pos			�J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ChangeSortType( ZKNSEARCHMAIN_WORK * wk, u32 pos )
{
	u32	list_pos = ZKNSEARCHMAIN_GetSortType( wk, FRAMELIST_GetScrollCount(wk->lwk)+pos );

	if( wk->dat->sort->type1 == ZKNCOMM_LIST_SORT_NONE ){
		wk->dat->sort->type1 = list_pos;
	}else if( wk->dat->sort->type2 == ZKNCOMM_LIST_SORT_NONE ){
		if( wk->dat->sort->type1 == list_pos ){
			wk->dat->sort->type1 = ZKNCOMM_LIST_SORT_NONE;
		}else{
			wk->dat->sort->type2 = list_pos;
		}
	}else{
		if( wk->dat->sort->type1 == list_pos ){
			wk->dat->sort->type1 = wk->dat->sort->type2;
			wk->dat->sort->type2 = ZKNCOMM_LIST_SORT_NONE;
		}else if( wk->dat->sort->type2 == list_pos ){
			wk->dat->sort->type2 = ZKNCOMM_LIST_SORT_NONE;
		}else{
			wk->dat->sort->type1 = wk->dat->sort->type2;
			wk->dat->sort->type2 = list_pos;
		}
	}
	FRAMELIST_SetCursorPos( wk->lwk, pos );
	ZKNSEARCHBMP_PutTypeItem( wk );
	ZKNSEARCHOBJ_ChangeTypeMark( wk, wk->dat->sort->type1, wk->dat->sort->type2 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�[�^���Z�b�g�F�^�C�v
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ResetSortType( ZKNSEARCHMAIN_WORK * wk )
{
	wk->dat->sort->type1 = ZKNCOMM_LIST_SORT_NONE;
	wk->dat->sort->type2 = ZKNCOMM_LIST_SORT_NONE;
	ZKNSEARCHBMP_PutTypeItem( wk );
	ZKNSEARCHOBJ_VanishMark( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�[�^�؂�ւ��F�F
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		pos			�J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ChangeSortColor( ZKNSEARCHMAIN_WORK * wk, u32 pos )
{
	u32	list_pos = FRAMELIST_GetScrollCount( wk->lwk ) + pos;

	if( wk->dat->sort->color == list_pos ){
		wk->dat->sort->color = ZKNCOMM_LIST_SORT_NONE;
		ZKNSEARCHOBJ_ChangeMark( wk, pos, FALSE );
	}else{
		wk->dat->sort->color = list_pos;
		ZKNSEARCHOBJ_ChangeMark( wk, pos, TRUE );
	}
	FRAMELIST_SetCursorPos( wk->lwk, pos );
	ZKNSEARCHBMP_PutColorItem( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�[�^���Z�b�g�F�F
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ResetSortColor( ZKNSEARCHMAIN_WORK * wk )
{
	wk->dat->sort->color = ZKNCOMM_LIST_SORT_NONE;
	ZKNSEARCHBMP_PutColorItem( wk );
	ZKNSEARCHOBJ_VanishMark( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�[�^�؂�ւ��F�t�H����
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		pos			�J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ChangeSortForm( ZKNSEARCHMAIN_WORK * wk, u32 pos )
{
	u32	list_pos = FRAMELIST_GetScrollCount( wk->lwk ) + pos;

	if( wk->dat->sort->form == list_pos ){
		wk->dat->sort->form = ZKNCOMM_LIST_SORT_NONE;
		ZKNSEARCHOBJ_ChangeFormMark( wk, pos, FALSE );
	}else{
		wk->dat->sort->form = list_pos;
		ZKNSEARCHOBJ_ChangeFormMark( wk, pos, TRUE );
	}
	FRAMELIST_SetCursorPos( wk->lwk, pos );
	ZKNSEARCHOBJ_PutFormListNow( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�[�^���Z�b�g�F�t�H����
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ResetSortForm( ZKNSEARCHMAIN_WORK * wk )
{
	wk->dat->sort->form = ZKNCOMM_LIST_SORT_NONE;
	ZKNSEARCHOBJ_PutFormListNow( wk );
	ZKNSEARCHOBJ_VanishMark( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�C���Z�b�g
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		next		�t�F�[�h��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int SetWipeIn( ZKNSEARCHMAIN_WORK * wk, int next )
{
	ZKNSEARCHMAIN_SetPalFadeSeq( wk, 16, 0 );
	wk->funcSeq = next;
	return MAINSEQ_WIPE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�A�E�g�Z�b�g
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		next		�t�F�[�h��̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int SetWipeOut( ZKNSEARCHMAIN_WORK * wk, int next )
{
	ZKNSEARCHMAIN_SetPalFadeSeq( wk, 0, 16 );
	wk->funcSeq = next;
	return MAINSEQ_WIPE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�^���A�j���ݒ�
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		mode		���[�h
 * @param		id			�{�^���h�c
 * @param		next		�A�j����̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int SetButtonAnm( ZKNSEARCHMAIN_WORK * wk, u8 mode, u8 id, int next )
{
	wk->btnMode = mode;
	wk->btnID   = id;
	wk->btnSeq  = 0;
	wk->btnCnt  = 0;
	wk->funcSeq = next;
	return MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�؂�ւ�
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		next		�؂�ւ���̃V�[�P���X
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int PageChange( ZKNSEARCHMAIN_WORK * wk, int next )
{
	wk->pageSeq = 0;
	wk->nextSeq = next;
	return MAINSEQ_EXIT_MENU;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�x�{�^���V���[�g�J�b�g�ݒ�
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static void SetShortCut( ZKNSEARCHMAIN_WORK * wk )
{
	if( GAMEDATA_GetShortCut( wk->dat->gamedata, SHORTCUT_ID_ZUKAN_SEARCH ) == TRUE ){
		GAMEDATA_SetShortCut( wk->dat->gamedata, SHORTCUT_ID_ZUKAN_SEARCH, FALSE );
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, APP_COMMON_BARICON_CHECK_OFF );
	}else{
		GAMEDATA_SetShortCut( wk->dat->gamedata, SHORTCUT_ID_ZUKAN_SEARCH, TRUE );
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, APP_COMMON_BARICON_CHECK_ON );
	}
}
