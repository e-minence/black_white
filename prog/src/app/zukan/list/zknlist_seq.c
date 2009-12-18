//============================================================================================
/**
 * @file		zknlist_seq.c
 * @brief		�}�Ӄ��X�g��� �V�[�P���X����
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	���W���[�����FZKNLISTSEQ
 */
//============================================================================================
#include <gflib.h>

#include "system/wipe.h"
#include "app/app_menu_common.h"

#include "zknlist_main.h"
#include "zknlist_seq.h"
#include "zknlist_obj.h"
#include "zknlist_bmp.h"
#include "zknlist_ui.h"
#include "zknlist_bgwfrm.h"


//============================================================================================
//	�萔��`
//============================================================================================

// ���C���V�[�P���X
enum {
	MAINSEQ_INIT = 0,
	MAINSEQ_RELEASE,
	MAINSEQ_WIPE,

	MAINSEQ_MAIN,
	MAINSEQ_LIST_WAIT,
	MAINSEQ_LIST_SCROLL,
	MAINSEQ_LIST_MOVE_LEFT,
	MAINSEQ_LIST_MOVE_RIGHT,
	MAINSEQ_BUTTON_ANM,

	MAINSEQ_END_SET,
	MAINSEQ_END,
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static int MainSeq_Init( ZKNLISTMAIN_WORK * wk );
static int MainSeq_Release( ZKNLISTMAIN_WORK * wk );
static int MainSeq_Wipe( ZKNLISTMAIN_WORK * wk );
static int MainSeq_Main( ZKNLISTMAIN_WORK * wk );
static int MainSeq_ListWait( ZKNLISTMAIN_WORK * wk );
static int MainSeq_ListScroll( ZKNLISTMAIN_WORK * wk );
static int MainSeq_ListMoveLeft( ZKNLISTMAIN_WORK * wk );
static int MainSeq_ListMoveRight( ZKNLISTMAIN_WORK * wk );

static int MainSeq_ButtonAnm( ZKNLISTMAIN_WORK * wk );
static int MainSeq_EndSet( ZKNLISTMAIN_WORK * wk );

static int SetFadeIn( ZKNLISTMAIN_WORK * wk, int next );
static int SetFadeOut( ZKNLISTMAIN_WORK * wk, int next );
static int SetButtonAnm( ZKNLISTMAIN_WORK * wk, u32 id, u32 anm, int next );
static int SetInfoData( ZKNLISTMAIN_WORK * wk, int pos );
static void ScrollBaseBg( ZKNLISTMAIN_WORK * wk );

FS_EXTERN_OVERLAY(ui_common);


//============================================================================================
//	�O���[�o��
//============================================================================================

static const pZKNLIST_FUNC MainSeq[] = {
	MainSeq_Init,
	MainSeq_Release,
	MainSeq_Wipe,

	MainSeq_Main,
	MainSeq_ListWait,
	MainSeq_ListScroll,
	MainSeq_ListMoveLeft,
	MainSeq_ListMoveRight,
	MainSeq_ButtonAnm,

	MainSeq_EndSet,
};




BOOL ZKNLISTSEQ_MainSeq( ZKNLISTMAIN_WORK * wk )
{
	wk->mainSeq = MainSeq[wk->mainSeq]( wk );
	if( wk->mainSeq == MAINSEQ_END ){
		return FALSE;
	}

	ZKNLISTOBJ_AnmMain( wk );
	ZKNLISTBMP_PrintUtilTrans( wk );
	BGWINFRM_MoveMain( wk->wfrm );
	ScrollBaseBg( wk );

	return TRUE;
}


static int MainSeq_Init( ZKNLISTMAIN_WORK * wk )
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

	ZKNLISTMAIN_InitVram();
	ZKNLISTMAIN_InitBg();
	ZKNLISTMAIN_LoadBgGraphic();
	ZKNLISTMAIN_InitMsg( wk );

	ZKNLISTBMP_Init( wk );
	ZKNLISTOBJ_Init( wk );
	ZKNLISTBGWFRM_Init( wk );

	ZKNLISTBMP_PutPokeEntryStr( wk );

	ZKNLISTMAIN_MakeList( wk );

	ZKNLISTMAIN_SetBlendAlpha();

	ZKNLISTMAIN_InitVBlank( wk );

	return SetFadeIn( wk, MAINSEQ_MAIN );
}

static int MainSeq_Release( ZKNLISTMAIN_WORK * wk )
{
	ZKNLISTMAIN_ExitVBlank( wk );

	ZKNLISTMAIN_FreeList( wk );

	ZKNLISTBGWFRM_Exit( wk );
	ZKNLISTOBJ_Exit( wk );
	ZKNLISTBMP_Exit( wk );

	ZKNLISTMAIN_ExitMsg( wk );
	ZKNLISTMAIN_ExitBg();

	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );

	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

	return MAINSEQ_END;
}

static int MainSeq_Wipe( ZKNLISTMAIN_WORK * wk )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		return wk->wipeSeq;
	}
	return MAINSEQ_WIPE;
}


static void SetListScrollSpeed( ZKNLISTMAIN_WORK * wk )
{
	if( wk->listRepeat < 40 ){
		wk->listRepeat++;
	}
	if( wk->listRepeat == 40 ){
		wk->listSpeed = 24;
	}else if( wk->listRepeat >= 32 ){
		wk->listSpeed = 12;
	}else if( wk->listRepeat >= 24 ){
		wk->listSpeed = 8;
	}else if( wk->listRepeat >= 16 ){
		wk->listSpeed = 6;
	}else if( wk->listRepeat >= 8 ){
		wk->listSpeed = 4;
	}else{
		wk->listSpeed = 3;
	}
}

static int MainSeq_Main( ZKNLISTMAIN_WORK * wk )
{
	u32	ret;
	
	// ���X�g�̃L�[����
	switch( ZKNLISTMAIN_Main( wk->list ) ){
	case ZKNLISTMAIN_LIST_MOVE_UP:
	case ZKNLISTMAIN_LIST_MOVE_DOWN:
		wk->listConut = 8;
		return MAINSEQ_LIST_WAIT;

	case ZKNLISTMAIN_LIST_MOVE_LEFT:
	case ZKNLISTMAIN_LIST_MOVE_RIGHT:
		wk->listConut = 4;
		return MAINSEQ_LIST_WAIT;

	case ZKNLISTMAIN_LIST_MOVE_SCROLL_UP:
		SetListScrollSpeed( wk );
		wk->listConut = 0;
		wk->listScroll = GFL_BG_SCROLL_Y_DEC;
		return MAINSEQ_LIST_SCROLL;

	case ZKNLISTMAIN_LIST_MOVE_SCROLL_DOWN:
		SetListScrollSpeed( wk );
		wk->listConut = 0;
		wk->listScroll = GFL_BG_SCROLL_Y_INC;
		return MAINSEQ_LIST_SCROLL;
	}

	wk->listRepeat = 0;
	
	ret = ZKNLISUI_ListMain( wk );

	switch( ret ){
	case ZKNLISTUI_ID_LIST:				// 00: ���X�g
	case ZKNLISTUI_ID_RAIL:				// 01: ���[��
		break;

	case ZKNLISTUI_ID_POKE:				// 02: �|�P�������ʊG
		return SetInfoData( wk, 0 );

	case ZKNLISTUI_ID_ICON1:			// 03: �|�P�����A�C�R��
	case ZKNLISTUI_ID_ICON2:			// 04: �|�P�����A�C�R��
	case ZKNLISTUI_ID_ICON3:			// 05: �|�P�����A�C�R��
	case ZKNLISTUI_ID_ICON4:			// 06: �|�P�����A�C�R��
	case ZKNLISTUI_ID_ICON5:			// 07: �|�P�����A�C�R��
	case ZKNLISTUI_ID_ICON6:			// 08: �|�P�����A�C�R��
	case ZKNLISTUI_ID_ICON7:			// 09: �|�P�����A�C�R��
		return SetInfoData( wk, ret );

	case ZKNLISTUI_ID_START:			// 10: �X�^�[�g
		wk->dat->retMode = ZKNLIST_RET_SEARCH;
		return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_START, ZKNLISTOBJ_ANM_START_ANM, MAINSEQ_END_SET );

	case ZKNLISTUI_ID_SELECT:			// 11: �Z���N�g
		return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_SELECT, ZKNLISTOBJ_ANM_SELECT_ANM, MAINSEQ_MAIN );

	case ZKNLISTUI_ID_LEFT:				// 12: ��
		return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT_ON, MAINSEQ_LIST_MOVE_LEFT );

	case ZKNLISTUI_ID_RIGHT:			// 13: �E
		return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT_ON, MAINSEQ_LIST_MOVE_RIGHT );

	case ZKNLISTUI_ID_Y:					// 14: �x
		break;

	case ZKNLISTUI_ID_X:					// 15: �w
		wk->dat->retMode = ZKNLIST_RET_EXIT_X;
		return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_EXIT, APP_COMMON_BARICON_EXIT_ON, MAINSEQ_END_SET );

	case ZKNLISTUI_ID_RETURN:			// 16: �߂�
		wk->dat->retMode = ZKNLIST_RET_EXIT;
		return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON, MAINSEQ_END_SET );

	case ZKNLISTUI_ID_LIST_UP:		// ��L�[
	case ZKNLISTUI_ID_LIST_DOWN:	// ���L�[
		break;
	}

	return MAINSEQ_MAIN;
}

static int MainSeq_ListWait( ZKNLISTMAIN_WORK * wk )
{
	wk->listConut--;
	if( wk->listConut == 0 ){
		return MAINSEQ_MAIN;
	}
	return MAINSEQ_LIST_WAIT;
}

static int MainSeq_ListScroll( ZKNLISTMAIN_WORK * wk )
{
	GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, wk->listScroll, wk->listSpeed );
	GFL_BG_SetScrollReq( GFL_BG_FRAME2_S, wk->listScroll, wk->listSpeed );
	if( wk->listScroll == GFL_BG_SCROLL_Y_DEC ){
		ZKNLISTOBJ_ScrollPokeIcon( wk, wk->listSpeed );
	}else{
		ZKNLISTOBJ_ScrollPokeIcon( wk, -wk->listSpeed );
	}

	wk->listConut += wk->listSpeed;
	if( wk->listConut == 24 ){
		ZKNLISTMAIN_PutListCursor( wk, 2, ZKNLISTMAIN_GetListPos(wk->list) );
		ZKNLISTOBJ_ChgListPosAnm( wk, ZKNLISTMAIN_GetListPos(wk->list), TRUE );
		ZKNLISTOBJ_PutListPosPokeGra( wk, ZKNLISTMAIN_GetListCursorPos(wk->list) );
		return MAINSEQ_MAIN;
	}
	return MAINSEQ_LIST_SCROLL;
}

static int MainSeq_ListMoveLeft( ZKNLISTMAIN_WORK * wk )
{
	ZKNLISTMAIN_MoveLeft( wk->list );
	return MAINSEQ_MAIN;
}

static int MainSeq_ListMoveRight( ZKNLISTMAIN_WORK * wk )
{
	ZKNLISTMAIN_MoveRight( wk->list );
	return MAINSEQ_MAIN;
}

static int MainSeq_ButtonAnm( ZKNLISTMAIN_WORK * wk )
{
	if( ZKNLISTOBJ_CheckAnm( wk, wk->buttonID ) == FALSE ){
		return wk->nextSeq;
	}	
	return MAINSEQ_BUTTON_ANM;
}

static int MainSeq_EndSet( ZKNLISTMAIN_WORK * wk )
{
	return SetFadeOut( wk, MAINSEQ_RELEASE );
}






static int SetFadeIn( ZKNLISTMAIN_WORK * wk, int next )
{
	ZKNCOMM_SetFadeIn( HEAPID_ZUKAN_LIST );
	wk->wipeSeq = next;
	return MAINSEQ_WIPE;
}

static int SetFadeOut( ZKNLISTMAIN_WORK * wk, int next )
{
	ZKNCOMM_SetFadeOut( HEAPID_ZUKAN_LIST );
	wk->wipeSeq = next;
	return MAINSEQ_WIPE;
}

static int SetButtonAnm( ZKNLISTMAIN_WORK * wk, u32 id, u32 anm, int next )
{
	ZKNLISTOBJ_SetAutoAnm( wk, id, anm );
	wk->buttonID = id;
	wk->nextSeq  = next;
	return MAINSEQ_BUTTON_ANM;
}

static int SetInfoData( ZKNLISTMAIN_WORK * wk, int pos )
{
//	wk->dat->retMons = pos;
	wk->dat->retMode = ZKNLIST_RET_INFO;
	return SetFadeOut( wk, MAINSEQ_RELEASE );
}




#define	BASEBG_SCROLL_VAL		( 1 )

static void ScrollBaseBg( ZKNLISTMAIN_WORK * wk )
{
	wk->bgScroll++;
	if( wk->bgScroll == ZKNCOMM_BG_SCROLL_WAIT ){
		GFL_BG_SetScrollReq( GFL_BG_FRAME3_M, GFL_BG_SCROLL_X_INC, BASEBG_SCROLL_VAL );
		GFL_BG_SetScrollReq( GFL_BG_FRAME3_S, GFL_BG_SCROLL_X_INC, BASEBG_SCROLL_VAL );
		wk->bgScroll = 0;
	}
}

