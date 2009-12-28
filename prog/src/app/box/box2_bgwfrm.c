//============================================================================================
/**
 * @file		box2_bgwfrm.c
 * @brief		�{�b�N�X��� BG�E�B���h�E�t���[���֘A
 * @author	Hiroyuki Nakamura
 * @date		09.10.07
 *
 *	���W���[�����FBOX2BGWFRM
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/main.h"

#include "box2_main.h"
#include "box2_bmp.h"
#include "box2_obj.h"
#include "box2_bgwfrm.h"
#include "box_gra.naix"


//============================================================================================
//	�萔��`
//============================================================================================

// �}�[�L���O
#define	MARKING_FRM_SX	( 11 )						// �}�[�L���O�t���[���w�T�C�Y
#define	MARKING_FRM_SY	( 15 )						// �}�[�L���O�t���[���x�T�C�Y
//#define	MARKING_FRM_PX	( 32 - MARKING_FRM_SX )		// �}�[�L���O�t���[���w�\�����W
//#define	MARKING_FRM_PY	( 26 )						// �}�[�L���O�t���[���w�\�����W

//#define	MARKING_ON_CHR	( 0x2b )					// �}�[�L���O�L�����J�n�ʒu ( ON )
//#define	MARKING_OFF_CHR	( 0x0b )					// �}�[�L���O�L�����J�n�ʒu ( OFF )

// �莝���|�P�����t���[��
#define	PARTYPOKE_FRM_SX	( BOX2BGWFRM_PARTYPOKE_SX )		// �莝���|�P�����t���[���w�T�C�Y
#define	PARTYPOKE_FRM_SY	( 15 )		// �莝���|�P�����t���[���x�T�C�Y
#define	PARTYPOKE_FRM_PX	( 2 )		// �莝���|�P�����t���[���w�\�����W
#define	PARTYPOKE_FRM_PY	( 21 )		// �莝���|�P�����t���[���x�\�����W
#define	WINFRM_PARTYPOKE_LX	( BOX2BGWFRM_PARTYPOKE_LX )
#define	WINFRM_PARTYPOKE_RX	( 21 )
#define	WINFRM_PARTYPOKE_PY	( BOX2BGWFRM_PARTYPOKE_PY )
#define	WINFRM_PARTYPOKE_INIT_PY	( 24 )
#define	WINFRM_PARTYPOKE_RET_PX		( 24 )
#define	WINFRM_PARTYPOKE_RET_PY		( 15 )

// �{�b�N�X�ړ��t���[��
#define BOXMOVE_FRM_SX			( 11 )		// �{�b�N�X�ړ��t���[���w�T�C�Y
#define BOXMOVE_FRM_SY			( 21 )		// �{�b�N�X�ړ��t���[���x�T�C�Y
#define BOXMV_PTOUT_FRM_SX	( 32 )		// �{�b�N�X�ړ��t���[���w�T�C�Y
#define BOXMV_PTOUT_FRM_SY	( 7 )		// �{�b�N�X�ړ��t���[���x�T�C�Y

// �ǎ��ύX�t���[��
#define WPCHG_FRM_SX		( 32 )		// �{�b�N�X�ړ��t���[���w�T�C�Y
#define WPCHG_FRM_SY		( 7 )		// �{�b�N�X�ړ��t���[���x�T�C�Y

// BGWIN�t���[���f�[�^
#define	SUBDISP_ITEM_FRM_SX			( 32 )
#define	SUBDISP_ITEM_FRM_SY			( 9 )
#define	SUBDISP_ITEM_FRM_IN_PX	( 0 )
#define	SUBDISP_ITEM_FRM_IN_PY	( 24 )
#define	SUBDISP_WAZA_FRM_SX			( 12 )
#define	SUBDISP_WAZA_FRM_SY			( 10 )
#define	SUBDISP_WAZA_FRM_IN_PX	( 32 )
#define	SUBDISP_WAZA_FRM_IN_PY	( 12 )

// �|�P�����I�����j���[�t���[���f�[�^
#define	WINFRM_MENU_INIT_PX	( 32 )		// �\���w���W
#define	WINFRM_MENU_PX		( 21 )		// �\���w���W
#define	WINFRM_MENU_PY		( 2 )		// �\���x���W
#define	WINFRM_MENU_SY		( 3 )		// �\���x�T�C�Y

//�u�{�b�N�X�����肩����v�{�^���t���[���f�[�^
#define	WINFRM_BOXCHG_BTN_PX			( 0 )
#define	WINFRM_BOXCHG_BTN_PY			( 21 )
#define	WINFRM_BOXCHG_BTN_INIT_PY	( 24 )

//�u�x�悤�����݂�v�{�^���t���[���f�[�^
#define	WINFRM_Y_ST_BTN_PX			( 0 )
#define	WINFRM_Y_ST_BTN_PY			( 21 )
#define	WINFRM_Y_ST_BTN_INIT_PY	( 24 )

//�u�Ă����|�P�����v�t���[���f�[�^
#define	WINFRM_TEMOCHI_PX		( 0 )
#define	WINFRM_TEMOCHI_PY		( 21 )
//#define	WINFRM_TEMOCHI_OUT_PY	( 24 )
//�u�{�b�N�X���X�g�v�t���[���f�[�^
#define	WINFRM_BOXLIST_PX			( 0 )
#define	WINFRM_BOXLIST_PY			( 21 )
//#define	WINFRM_IDOU_OUT_PY		( 24 )
//�u���ǂ�v�t���[���f�[�^
#define	WINFRM_MODORU_PX		( 24 )
#define	WINFRM_MODORU_PY		( 21 )
#define	WINFRM_MODORU_OUT_PY	( 24 )

// �}�[�L���O�z�u�f�[�^
//#define	MARK_DEF_PX		( 3 )
//#define	MARK_DEF_PY		( 1 )
//#define	MARK_DEF_SX		( 4 )
//#define	MARK_DEF_SY		( 3 )

// �}�[�L���O�t���[���f�[�^
#define	WINFRM_MARK_PX			( 21 )
#define	WINFRM_MARK_PY			( 5 )
#define	WINFRM_MARK_IN_START_PY	( 24 )
#define	WINFRM_MARK_MV_CNT		( WINFRM_MARK_IN_START_PY - WINFRM_MARK_PY )

// �{�b�N�X�ړ����j���[�t���[���f�[�^
#define	WINFRM_BOXMV_MENU_PX		( 21 )
#define	WINFRM_BOXMV_MENU_PY		( 14 )
#define	WINFRM_BOXMV_MENU_INIT_PX	( 32 )
#define	WINFRM_BOXMV_MENU_SX		( 11 )

// �g���C�I���t���[���f�[�^
#define	WINFRM_TRAYMOVE_IN_PX		( 32 )
#define	WINFRM_TRAYMOVE_IN_PY		( 0 )
#define	WINFRM_TRAYMOVE_OUT_PX	( 21 )
#define	WINFRM_TRAYMOVE_OUT_PY	( 0 )

// �{�b�N�X�e�[�}�ύX���j���[�t���[���f�[�^
#define	WINFRM_BOXTHEMA_MENU_SX		( 11 )
//#define	WINFRM_BOXTHEMA_MENU_SY		( 14 )
#define	WINFRM_BOXTHEMA_MENU_PX		( WINFRM_BOXMV_MENU_PX )
#define	WINFRM_BOXTHEMA_MENU1_PY	( 8 )
#define	WINFRM_BOXTHEMA_MENU2_PY	( WINFRM_BOXTHEMA_MENU1_PY + 6 )
#define	WINFRM_BOXTHEMA_MENU3_PY	( WINFRM_BOXTHEMA_MENU2_PY + 3 )
#define	WINFRM_BOXTHEMA_MENU_OUT_PX	( 32 )

// �ǎ��ύX�t���[���f�[�^
#define	WINFRM_WALLPAPER_PX			( 0 )
#define	WINFRM_WALLPAPER_IN_PY		( -WPCHG_FRM_SY )
#define	WINFRM_WALLPAPER_OUT_PY		( 0 )

//#define	BOXPARTY_BTN_SPD	( 8 )	// �u�Ă����|�P�����v�u�|�P�������ǂ��v�{�^���X�N���[�����x
#define	BOXPARTY_BTN_CNT	( 3 )	// �u�Ă����|�P�����v�u�|�P�������ǂ��v�{�^���X�N���[���J�E���g


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void FrameArcLoad( BGWINFRM_WORK * wk, u32 index, u32 dataIdx );
static void PokeMenuInitPosSet( BGWINFRM_WORK * wk );
static void InitTouchBar( BOX2_SYS_WORK * syswk );



//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�t���[�����[�N�쐬�i�S�́j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_Init( BOX2_SYS_WORK * syswk )
{
	BOX2_APP_WORK * appwk = syswk->app;

	appwk->wfrm = BGWINFRM_Create( BGWINFRM_TRANS_VBLANK, BOX2MAIN_WINFRM_MAX, HEAPID_BOX_APP );

	BOX2BMP_PokeMenuBgFrmWkMake( appwk );				// ���j���[
	InitTouchBar( syswk );											// �^�b�`�o�[

	BOX2BMP_TemochiButtonBgFrmWkMake( syswk );	//�u�Ă����|�P�����vor�u�o�g���{�b�N�X�v
	BOX2BMP_BoxListButtonBgFrmWkMake( syswk );	//�u�{�b�N�X���X�g�v

	// �莝���|�P�����t���[��
	BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_PARTY, GFL_BG_FRAME1_M, PARTYPOKE_FRM_SX, PARTYPOKE_FRM_SY );
	if( syswk->dat->callMode == BOX_MODE_BATTLE ){
		FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_PARTY, NARC_box_gra_box_poke_bg_lz_NSCR );
	}else{
		FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_PARTY, NARC_box_gra_box_poke_bg2_lz_NSCR );
	}

	// �}�[�L���O�t���[��
	BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_MARK, GFL_BG_FRAME1_M, MARKING_FRM_SX, MARKING_FRM_SY );

	// �{�b�N�X�ړ��t���[��
	BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, GFL_BG_FRAME1_M, BOXMOVE_FRM_SX, BOXMOVE_FRM_SY );
	FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, NARC_box_gra_box_move_bg_lz_NSCR );






	// �{�b�N�X�ړ��t���[��
//	BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_BOXMV_MENU, GFL_BG_FRAME1_M, BOX2BMP_BOXMVMENU_SX, BOX2BMP_BOXMVMENU_SY );
/*
	if( syswk->dat->callMode == BOX_MODE_AZUKERU ){
		BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, GFL_BG_FRAME0_M, BOXMV_PTOUT_FRM_SX, BOXMV_PTOUT_FRM_SY );
		FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, NARC_box_gra_box_partyout_bg_lz_NSCR );
	}else{
		BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, GFL_BG_FRAME1_M, BOXMOVE_FRM_SX, BOXMOVE_FRM_SY );
		FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, NARC_box_gra_box_move_bg_lz_NSCR );
	}
*/


/*	���ʁ@�Z�E�A�C�e���t���[��
	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		BGWINFRM_Add(
			appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP,
			GFL_BG_FRAME2_S, SUBDISP_ITEM_FRM_SX, SUBDISP_ITEM_FRM_SY );
		BGWINFRM_FramePut(
			appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP,
			SUBDISP_ITEM_FRM_IN_PX, SUBDISP_ITEM_FRM_IN_PY );
		FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP, NARC_box_gra_sub_item_frm_lz_NSCR );
	}else{
		BGWINFRM_Add(
			appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP,
			GFL_BG_FRAME2_S, SUBDISP_WAZA_FRM_SX, SUBDISP_WAZA_FRM_SY );
		BGWINFRM_FramePut(
			appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP,
			SUBDISP_WAZA_FRM_IN_PX, SUBDISP_WAZA_FRM_IN_PY );
		FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP, NARC_box_gra_sub_waza_frm_lz_NSCR );
	}
*/

	FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_MARK, NARC_box_gra_box_mark_bg_lz_NSCR );
	BOX2BMP_MarkingButtonFrmPut( syswk );

	PokeMenuInitPosSet( appwk->wfrm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�t���[�����[�N���
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_Exit( BOX2_APP_WORK * appwk )
{
	BGWINFRM_Exit( appwk->wfrm );
}

//--------------------------------------------------------------------------------------------
/**
 * �a�f�t���[���ɃA�[�J�C�u�̃O���t�B�b�N��ݒ�
 *
 * @param	wk				�a�f�E�B���h�E�t���[�����[�N
 * @param	ah				�A�[�J�C�u�n���h��
 * @param	dataIdx		�f�[�^�C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void FrameArcLoad( BGWINFRM_WORK * wk, u32 index, u32 dataIdx )
{
	BGWINFRM_FrameSetArc( wk, index, ARCID_BOX2_GRA, dataIdx, TRUE );
}


//============================================================================================
//	�莝���|�P�����t���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���\��
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PartyPokeFramePut( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_PARTY, WINFRM_PARTYPOKE_LX, WINFRM_PARTYPOKE_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���\���i�E�j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PartyPokeFramePutRight( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_PARTY, WINFRM_PARTYPOKE_RX, WINFRM_PARTYPOKE_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[����\��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2BGWFRM_PartyPokeFrameOff( BOX2_SYS_WORK * syswk )
{
	GFL_BG_FillScreen(
		GFL_BG_FRAME1_M, 0,
		PARTYPOKE_FRM_PX, PARTYPOKE_FRM_PY,
		PARTYPOKE_FRM_SX, PARTYPOKE_FRM_SY, GFL_BG_SCRWRT_PALNL );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[�������ʒu�ݒ�i���j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PartyPokeFrameInitPutLeft( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_PARTY, WINFRM_PARTYPOKE_LX, WINFRM_PARTYPOKE_INIT_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[�������ʒu�ݒ�i�E�j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PartyPokeFrameInitPutRight( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_PARTY, WINFRM_PARTYPOKE_RX, WINFRM_PARTYPOKE_INIT_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���C���Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PartyPokeFrameInSet( BGWINFRM_WORK * wk )
{
	u16	i;
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_PARTY, &px, &py );
	if( py != WINFRM_PARTYPOKE_PY ){
		BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, 0, -1, py-WINFRM_PARTYPOKE_PY );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���A�E�g�Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PartyPokeFrameOutSet( BGWINFRM_WORK * wk )
{
	u16	i;
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_PARTY, &px, &py );
	if( py != WINFRM_PARTYPOKE_INIT_PY ){
		BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, 0, 1, WINFRM_PARTYPOKE_INIT_PY-py );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���E�ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PartyPokeFrameRightMoveSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, 1, 0, BOX2MAIN_PARTYPOKE_FRM_H_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[�����ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PartyPokeFrameLeftMoveSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, -1, 0, BOX2MAIN_PARTYPOKE_FRM_H_CNT );
/*
	BGWINFRM_FramePut(
		wk, BOX2MAIN_WINFRM_RET_BTN,
		WINFRM_PARTYPOKE_RET_PX+BOX2MAIN_PARTYPOKE_FRM_H_CNT,
		WINFRM_PARTYPOKE_PY+WINFRM_PARTYPOKE_RET_PY );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_RET_BTN, -1, 0, BOX2MAIN_PARTYPOKE_FRM_H_CNT );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���ړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"TRUE = �ړ���"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2BGWFRM_PartyPokeFrameMove( BOX2_SYS_WORK * syswk )
{
	u32	party_mv;
	s8	x1, y1, x2, y2;

	BGWINFRM_PosGet( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY, &x1, &y1 );

	party_mv = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY );

	BGWINFRM_PosGet( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY, &x2, &y2 );

	if( x1 != x2 || y1 != y2 ){
		BOX2OBJ_PartyPokeIconScroll( syswk );
	}

	if( party_mv == 0 ){
		return FALSE;
	}

	return TRUE;
}

// �莝���|�P�����t���[�����E�ɂ��邩
BOOL BOX2BGWFRM_CheckPartyPokeFrameRight( BGWINFRM_WORK * wk )
{
	s8	x, y;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_PARTY, &x, &y );
	if( x == WINFRM_PARTYPOKE_RX && y == WINFRM_PARTYPOKE_PY ){
		return TRUE;
	}
	return FALSE;
}

// �莝���|�P�����t���[�������ɂ��邩
BOOL BOX2BGWFRM_CheckPartyPokeFrameLeft( BGWINFRM_WORK * wk )
{
	s8	x, y;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_PARTY, &x, &y );
	if( x == WINFRM_PARTYPOKE_LX && y == WINFRM_PARTYPOKE_PY ){
		return TRUE;
	}
	return FALSE;
}


//============================================================================================
//	�|�P�������j���[�t���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�������j���[�{�^������ʊO�ɔz�u
 *
 * @param		wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeMenuInitPosSet( BGWINFRM_WORK * wk )
{
	u32	i;

	for( i=0; i<6; i++ ){
		BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MENU1+i, WINFRM_MENU_INIT_PX, WINFRM_MENU_PY+i*WINFRM_MENU_SY );
	}
//	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_CLOSE_BTN, WINFRM_MODORU_PX, WINFRM_MODORU_PY+BOXPARTY_BTN_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�{�^������ʓ��ɔz�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PokeMenuOpenPosSet( BGWINFRM_WORK * wk )
{
	u32	i;

	for( i=0; i<6; i++ ){
		BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MENU1+i, WINFRM_MENU_PX, WINFRM_MENU_PY+i*WINFRM_MENU_SY );
	}
//	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_CLOSE_BTN, WINFRM_MODORU_PX, WINFRM_MODORU_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�{�^����\��
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PokeMenuOff( BGWINFRM_WORK * wk )
{
	u32	i;

	for( i=0; i<6; i++ ){
		BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_MENU1+i );
		BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MENU1+i, WINFRM_MENU_INIT_PX, WINFRM_MENU_PY+i*WINFRM_MENU_SY );
	}
//	BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_CLOSE_BTN );
//	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_CLOSE_BTN, WINFRM_MODORU_PX, WINFRM_MODORU_PY+BOXPARTY_BTN_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PokeMenuInSet( BGWINFRM_WORK * wk )
{
	u16	i;
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_MENU1, &px, &py );
	if( px == WINFRM_MENU_PX ){ return; }

	for( i=0; i<6; i++ ){
		BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MENU1+i, -1, 0, px-WINFRM_MENU_PX );
	}

//	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_CLOSE_BTN, &px, &py );
//	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_CLOSE_BTN, 0, -1, py-WINFRM_MODORU_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PokeMenuOutSet( BGWINFRM_WORK * wk )
{
	u16	i;
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_MENU1, &px, &py );
	if( px == WINFRM_MENU_INIT_PX ){ return; }

	for( i=0; i<6; i++ ){
		BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MENU1+i, 1, 0, WINFRM_MENU_INIT_PX-px );
	}

//	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_CLOSE_BTN, &px, &py );
//	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_CLOSE_BTN, 0, 1, WINFRM_MODORU_PY+BOXPARTY_BTN_CNT-py );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�ړ��`�F�b�N
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @retval	"TRUE = �ړ���"
 * @retval	"FLASE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2BGWFRM_PokeMenuMoveMain( BGWINFRM_WORK * wk )
{
	u32		i;
	BOOL	ret;
	
	ret = FALSE;

	for( i=0; i<6; i++ ){
		if( BGWINFRM_MoveOne( wk, BOX2MAIN_WINFRM_MENU1+i ) == 1 ){
			ret = TRUE;
		}
	}
/*
	if( BGWINFRM_MoveOne( wk, BOX2MAIN_WINFRM_CLOSE_BTN ) == 1 ){
		ret = TRUE;
	}
*/

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�z�u�`�F�b�N
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @retval	"TRUE = �z�u�ς�"
 * @retval	"FLASE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2BGWFRM_PokeMenuPutCheck( BGWINFRM_WORK * wk )
{
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_MENU1, &px, &py );
	if( px == WINFRM_MENU_INIT_PX ){
		return FALSE;
	}
	return TRUE;
}





//============================================================================================
//	�^�b�`�o�[
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�o�[������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitTouchBar( BOX2_SYS_WORK * syswk )
{
	u16 * scr = GFL_BG_GetScreenBufferAdrs( GFL_BG_FRAME0_M );

	scr = &scr[BOX2MAIN_TOUCH_BAR_PY*BOX2MAIN_TOUCH_BAR_SX+BOX2MAIN_TOUCH_BAR_PX];
	BGWINFRM_Add(
		syswk->app->wfrm, BOX2MAIN_WINFRM_TOUCH_BAR,
		GFL_BG_FRAME0_M, BOX2MAIN_TOUCH_BAR_SX, BOX2MAIN_TOUCH_BAR_SY );
	BGWINFRM_FrameSet( syswk->app->wfrm, BOX2MAIN_WINFRM_TOUCH_BAR, scr );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�o�[�z�u
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PutTouchBar( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_TOUCH_BAR, BOX2MAIN_TOUCH_BAR_PX, BOX2MAIN_TOUCH_BAR_PY );
}


//============================================================================================
//	���̑�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�Ă����|�P�����v�{�^���z�u
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_TemochiButtonOn( BOX2_APP_WORK * appwk )
{
	BGWINFRM_FramePut( appwk->wfrm, BOX2MAIN_WINFRM_POKE_BTN, WINFRM_TEMOCHI_PX, WINFRM_TEMOCHI_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�Ă����|�P�����v�{�^����\��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_TemochiButtonOff( BOX2_APP_WORK * appwk )
{
	BGWINFRM_FrameOff( appwk->wfrm, BOX2MAIN_WINFRM_POKE_BTN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�{�b�N�X���X�g�v�{�^���z�u
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_BoxListButtonOn( BOX2_APP_WORK * appwk )
{
	BGWINFRM_FramePut( appwk->wfrm, BOX2MAIN_WINFRM_BOXLIST_BTN, WINFRM_BOXLIST_PX, WINFRM_BOXLIST_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P�������ǂ��v�{�^������ʊO�ɔz�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_BoxListButtonOff( BOX2_APP_WORK * appwk )
{
	BGWINFRM_FrameOff( appwk->wfrm, BOX2MAIN_WINFRM_BOXLIST_BTN );
}


//============================================================================================
//	�}�[�L���O�t���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �}�[�L���O�t���[���C���Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_MarkingFrameInSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MARK, WINFRM_MARK_PX, WINFRM_MARK_IN_START_PY );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MARK, 0, -1, WINFRM_MARK_MV_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �}�[�L���O�t���[���A�E�g�Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_MarkingFrameOutSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MARK, 0, 1, WINFRM_MARK_MV_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �}�[�L���O�t���[���ړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"TRUE = �ړ���"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
int BOX2BGWFRM_MarkingFrameMove( BOX2_SYS_WORK * syswk )
{
	u32	mv;
	s8	x1, y1, x2, y2;

	BGWINFRM_PosGet( syswk->app->wfrm, BOX2MAIN_WINFRM_MARK, &x1, &y1 );

	mv = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_MARK );

	BGWINFRM_PosGet( syswk->app->wfrm, BOX2MAIN_WINFRM_MARK, &x2, &y2 );

	if( x1 != x2 || y1 != y2 ){
		BOX2OBJ_MarkingScroll( syswk );
	}

	return mv;
}


//============================================================================================
//	�g���C�I���t���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �g���C�I���t���[����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_BoxMoveFrmInSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MOVE, WINFRM_TRAYMOVE_IN_PX, WINFRM_TRAYMOVE_IN_PY );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MOVE, -1, 0, BOXMOVE_FRM_SX );
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�I���t���[����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_BoxMoveFrmOutSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MOVE, 1, 0, BOXMOVE_FRM_SX );
}

// �g���C�I���t���[�����\������Ă��邩
BOOL BOX2BGWFRM_CheckBoxMoveFrm( BGWINFRM_WORK * wk )
{
	s8	x, y;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_MOVE, &x, &y );
	if( x == WINFRM_TRAYMOVE_OUT_PX ){
		return TRUE;
	}
	return FALSE;
}



//============================================================================================
//	���ʁF�Z�t���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʋZ�t���[���C���Z�b�g
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2BGWFRM_SubDispWazaFrmInSet( BOX2_APP_WORK * appwk )
{
	if( appwk->subdisp_waza_put == 0 ){ return; }
	BGWINFRM_MoveInit( appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP, -1, 0, SUBDISP_WAZA_FRM_SX );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʋZ�t���[���A�E�g�Z�b�g
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2BGWFRM_SubDispWazaFrmOutSet( BOX2_APP_WORK * appwk )
{
	if( appwk->subdisp_waza_put == 0 ){ return; }
	BGWINFRM_MoveInit( appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP, 1, 0, SUBDISP_WAZA_FRM_SX );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʋZ�t���[����ʓ��ɔz�u
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2BGWFRM_SubDispWazaFrmInPosSet( BOX2_APP_WORK * appwk )
{
	if( appwk->subdisp_waza_put == 0 ){ return; }
	BGWINFRM_FrameOff( appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP );
	BGWINFRM_FramePut(
		appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP,
		SUBDISP_WAZA_FRM_IN_PX-SUBDISP_WAZA_FRM_SX, SUBDISP_WAZA_FRM_IN_PY );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʋZ�t���[����ʊO�ɔz�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2BGWFRM_SubDispWazaFrmOutPosSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_SUBDISP );
	BGWINFRM_FramePut(
		wk, BOX2MAIN_WINFRM_SUBDISP, SUBDISP_WAZA_FRM_IN_PX, SUBDISP_WAZA_FRM_IN_PY );
}
*/


//============================================================================================
//	���ʁF�������t���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʎ������t���[���C���Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2BGWFRM_SubDispItemFrmInSet( BGWINFRM_WORK * wk )
{
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_SUBDISP, &px, &py );
	if( py == SUBDISP_ITEM_FRM_IN_PY-SUBDISP_ITEM_FRM_SY ){
		return;
	}
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_SUBDISP, 0, -1, SUBDISP_ITEM_FRM_SY-(SUBDISP_ITEM_FRM_IN_PY-py) );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʎ������t���[���A�E�g�Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2BGWFRM_SubDispItemFrmOutSet( BGWINFRM_WORK * wk )
{
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_SUBDISP, &px, &py );
	if( py == SUBDISP_ITEM_FRM_IN_PY ){
		return;
	}
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_SUBDISP, 0, 1, SUBDISP_ITEM_FRM_IN_PY-py );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʎ������t���[������ʓ��ɔz�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2BGWFRM_SubDispItemFrmInPosSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_SUBDISP );
	BGWINFRM_FramePut(
		wk, BOX2MAIN_WINFRM_SUBDISP, SUBDISP_ITEM_FRM_IN_PX, SUBDISP_ITEM_FRM_IN_PY-SUBDISP_ITEM_FRM_SY );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʎ������t���[������ʊO�ɔz�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2BGWFRM_SubDispItemFrmOutPosSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_SUBDISP );
	BGWINFRM_FramePut(
		wk, BOX2MAIN_WINFRM_SUBDISP, SUBDISP_ITEM_FRM_IN_PX, SUBDISP_ITEM_FRM_IN_PY );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʃE�B���h�E����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2BGWFRM_SubDispWinFrmMove( BOX2_SYS_WORK * syswk )
{
	if( syswk->dat->callMode != BOX_MODE_ITEM ){
		BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_SUBDISP );
	}else{
		BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_SUBDISP );
		BOX2OBJ_ItemIconPutSub( syswk->app );
	}
}
*/
