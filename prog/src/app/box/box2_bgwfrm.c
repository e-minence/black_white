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
#include "box2_bgwfrm.h"
#include "box_gra.naix"


//============================================================================================
//	�萔��`
//============================================================================================

// �}�[�L���O
#define	MARKING_FRM_SX	( 11 )						// �}�[�L���O�t���[���w�T�C�Y
#define	MARKING_FRM_SY	( 18 )						// �}�[�L���O�t���[���x�T�C�Y
//#define	MARKING_FRM_PX	( 32 - MARKING_FRM_SX )		// �}�[�L���O�t���[���w�\�����W
//#define	MARKING_FRM_PY	( 26 )						// �}�[�L���O�t���[���w�\�����W
/*
#define	MARKING_ON_CHR	( 0x2b )					// �}�[�L���O�L�����J�n�ʒu ( ON )
#define	MARKING_OFF_CHR	( 0x0b )					// �}�[�L���O�L�����J�n�ʒu ( OFF )
*/
// �莝���|�P�����t���[��
#define	PARTYPOKE_FRM_SX	( 11 )		// �莝���|�P�����t���[���w�T�C�Y
#define	PARTYPOKE_FRM_SY	( 18 )		// �莝���|�P�����t���[���x�T�C�Y
//#define	PARTYPOKE_FRM_PX	( 2 )		// �莝���|�P�����t���[���w�\�����W
//#define	PARTYPOKE_FRM_PY	( 21 )		// �莝���|�P�����t���[���x�\�����W

// �{�b�N�X�ړ��t���[��
#define BOXMOVE_FRM_SX		( 32 )		// �{�b�N�X�ړ��t���[���w�T�C�Y
#define BOXMOVE_FRM_SY		( 6 )		// �{�b�N�X�ړ��t���[���x�T�C�Y
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
//#define	WINFRM_MENU_PX		( 21 )		// �\���w���W
#define	WINFRM_MENU_PY		( 5 )		// �\���x���W
#define	WINFRM_MENU_SY		( 3 )		// �\���x�T�C�Y

//�u���ǂ�v�t���[���f�[�^
#define	WINFRM_MODORU_PX		( 24 )
#define	WINFRM_MODORU_PY		( 21 )
//#define	WINFRM_MODORU_OUT_PY	( 24 )

//�u�{�b�N�X�����肩����v�{�^���t���[���f�[�^
#define	WINFRM_BOXCHG_BTN_PX			( 0 )
//#define	WINFRM_BOXCHG_BTN_PY			( 21 )
#define	WINFRM_BOXCHG_BTN_INIT_PY	( 24 )

//�u�x�悤�����݂�v�{�^���t���[���f�[�^
#define	WINFRM_Y_ST_BTN_PX			( 0 )
//#define	WINFRM_Y_ST_BTN_PY			( 21 )
#define	WINFRM_Y_ST_BTN_INIT_PY	( 24 )


//#define	BOXPARTY_BTN_SPD	( 8 )	// �u�Ă����|�P�����v�u�|�P�������ǂ��v�{�^���X�N���[�����x
#define	BOXPARTY_BTN_CNT	( 3 )	// �u�Ă����|�P�����v�u�|�P�������ǂ��v�{�^���X�N���[���J�E���g


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void FrameArcLoad( BGWINFRM_WORK * wk, u32 index, u32 dataIdx );
static void PokeMenuInitPosSet( BGWINFRM_WORK * wk );
static void BoxMoveButtonInitPut( BGWINFRM_WORK * wk );
static void YStatusButtonInitPut( BGWINFRM_WORK * wk );



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

// �ǎ��ύX�t���[��
	BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_WPCHG, GFL_BG_FRAME1_M, WPCHG_FRM_SX, WPCHG_FRM_SY );
	// �莝���|�P�����t���[��
	BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_PARTY, GFL_BG_FRAME1_M, PARTYPOKE_FRM_SX, PARTYPOKE_FRM_SY );
	// �}�[�L���O�t���[��
	BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_MARK, GFL_BG_FRAME0_M, MARKING_FRM_SX, MARKING_FRM_SY );
// �{�b�N�X�ړ��t���[��
	BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_BOXMV_MENU, GFL_BG_FRAME0_M, BOX2BMP_BOXMVMENU_SX, BOX2BMP_BOXMVMENU_SY );

	BOX2BMP_PokeMenuBgFrmWkMake( appwk );				// ���j���[

	BOX2BMP_TemochiButtonBgFrmWkMake( appwk );	//�u�v
	BOX2BMP_IdouButtonBgFrmWkMake( appwk );			//�u�v
	BOX2BMP_ModoruButtonBgFrmWkMake( appwk );		//�u�v
	BOX2BMP_ToziruButtonBgFrmWkMake( syswk );		//�u�v

	if( syswk->dat->mode == BOX_MODE_AZUKERU ){
		BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, GFL_BG_FRAME0_M, BOXMV_PTOUT_FRM_SX, BOXMV_PTOUT_FRM_SY );
		FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, NARC_box_gra_box_partyout_bg_lz_NSCR );
	}else{
		BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, GFL_BG_FRAME1_M, BOXMOVE_FRM_SX, BOXMOVE_FRM_SY );
		FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, NARC_box_gra_box_move_bg_lz_NSCR );
	}

	FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_WPCHG, NARC_box_gra_box_wpchg_bg_lz_NSCR );

	if( syswk->dat->mode == BOX_MODE_AZUKERU || syswk->dat->mode == BOX_MODE_TURETEIKU ){
		FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_PARTY, NARC_box_gra_box_poke_bg2_lz_NSCR );
	}else{
		BOX2BGWFRM_PartyPokeFrameLoadArrange( appwk->wfrm, BOX2MAIN_WINFRM_PARTY );
//		BOX2BMP_PartyCngButtonFrmPut( appwk );
	}

	if( syswk->dat->mode == BOX_MODE_ITEM ){
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

	FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_MARK, NARC_box_gra_box_mark_bg_lz_NSCR );
//	BOX2BMP_MarkingButtonFrmPut( syswk->app );

	PokeMenuInitPosSet( appwk->wfrm );
	BoxMoveButtonInitPut( appwk->wfrm );
	YStatusButtonInitPut( appwk->wfrm );
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
/*
void BOX2MAIN_PartyPokeFramePut( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_PARTY, WINFRM_PARTYPOKE_LX, WINFRM_PARTYPOKE_PY );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���\���i�E�j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_PartyPokeFramePutRight( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_PARTY, WINFRM_PARTYPOKE_RX, WINFRM_PARTYPOKE_PY );
}
*/

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
void BOX2MAIN_PartyPokeFrameOff( BOX2_SYS_WORK * syswk )
{
	GFL_BG_FillScreen(
//��[GS_CONVERT_TAG]
		syswk->app->bgl, BOX2MAIN_BGF_BTN_M, 0,
		PARTYPOKE_FRM_PX, PARTYPOKE_FRM_PY,
		PARTYPOKE_FRM_SX, PARTYPOKE_FRM_SY, GFL_BG_SCRWRT_PALNL );
//��[GS_CONVERT_TAG]
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
/*
void BOX2MAIN_PartyPokeFrameInitPutLeft( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_PARTY, WINFRM_PARTYPOKE_LX, WINFRM_PARTYPOKE_INIT_PY );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[�������ʒu�ݒ�i�E�j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_PartyPokeFrameInitPutRight( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_PARTY, WINFRM_PARTYPOKE_RX, WINFRM_PARTYPOKE_INIT_PY );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���C���Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_PartyPokeFrameInSet( BGWINFRM_WORK * wk )
{
	u16	i;
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_PARTY, &px, &py );
	if( py != WINFRM_PARTYPOKE_PY ){
		BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, 0, -1, py-WINFRM_PARTYPOKE_PY );
		if( px == WINFRM_PARTYPOKE_LX ){
			BGWINFRM_FramePut(
				wk, BOX2MAIN_WINFRM_RET_BTN, WINFRM_PARTYPOKE_RET_PX, py+WINFRM_PARTYPOKE_RET_PY );
			BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_RET_BTN, 0, -1, py-WINFRM_PARTYPOKE_PY );
		}
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���A�E�g�Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_PartyPokeFrameOutSet( BGWINFRM_WORK * wk )
{
	u16	i;
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_PARTY, &px, &py );
	if( py != WINFRM_PARTYPOKE_INIT_PY ){
		BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, 0, 1, WINFRM_PARTYPOKE_INIT_PY-py );
		if( px == WINFRM_PARTYPOKE_LX ){
			BGWINFRM_FramePut(
				wk, BOX2MAIN_WINFRM_RET_BTN, WINFRM_PARTYPOKE_RET_PX, py+WINFRM_PARTYPOKE_RET_PY );
			BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_RET_BTN, 0, 1, WINFRM_PARTYPOKE_INIT_PY-py );
		}
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����̂݃t���[���A�E�g�Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 *
 * @li	�ʏ�́u��߂�v���Z�b�g�����
 */
//--------------------------------------------------------------------------------------------
/*
static void PartyPokeFrameOnlyOutSet( BGWINFRM_WORK * wk )
{
	u16	i;
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_PARTY, &px, &py );
	if( py != WINFRM_PARTYPOKE_INIT_PY ){
		BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, 0, 1, WINFRM_PARTYPOKE_INIT_PY-py );
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����̂݃t���[���C���Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 *
 * @li	�ʏ�́u��߂�v���Z�b�g�����
 */
//--------------------------------------------------------------------------------------------
/*
static void PartyPokeFrameOnlyInSet( BGWINFRM_WORK * wk )
{
	u16	i;
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_PARTY, &px, &py );
	if( py != WINFRM_PARTYPOKE_PY ){
		BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, 0, -1, py-WINFRM_PARTYPOKE_PY );
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���E�ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_PartyPokeFrameRightMoveSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, 1, 0, BOX2MAIN_PARTYPOKE_FRM_H_CNT );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_RET_BTN, 1, 0, BOX2MAIN_PARTYPOKE_FRM_H_CNT );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���E�ړ��Z�b�g�i�u�|�P��������������v�p�j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_AzukeruPartyPokeFrameRightMoveSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, 1, 0, BOX2MAIN_PARTYPOKE_FRM_H_CNT );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[�����ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_PartyPokeFrameLeftMoveSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, -1, 0, BOX2MAIN_PARTYPOKE_FRM_H_CNT );

	BGWINFRM_FramePut(
		wk, BOX2MAIN_WINFRM_RET_BTN,
		WINFRM_PARTYPOKE_RET_PX+BOX2MAIN_PARTYPOKE_FRM_H_CNT,
		WINFRM_PARTYPOKE_PY+WINFRM_PARTYPOKE_RET_PY );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_RET_BTN, -1, 0, BOX2MAIN_PARTYPOKE_FRM_H_CNT );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[�����ړ��Z�b�g�i�u�|�P��������������v�p�j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_AzukeruPartyPokeFrameLeftMoveSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, -1, 0, BOX2MAIN_PARTYPOKE_FRM_H_CNT );
}
*/

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
/*
static BOOL PartyPokeFrameMove( BOX2_SYS_WORK * syswk )
{
	u32	party_mv, ret_mv;
	s8	x1, y1, x2, y2;
	BOOL	ret;

	BGWINFRM_PosGet( syswk->app->wfrmwk, BOX2MAIN_WINFRM_PARTY, &x1, &y1 );

	party_mv = BGWINFRM_MoveOne( syswk->app->wfrmwk, BOX2MAIN_WINFRM_PARTY );
	ret_mv   = BGWINFRM_MoveOne( syswk->app->wfrmwk, BOX2MAIN_WINFRM_RET_BTN );
	BGWINFRM_MoveOne( syswk->app->wfrmwk, BOX2MAIN_WINFRM_Y_ST_BTN );

	BGWINFRM_PosGet( syswk->app->wfrmwk, BOX2MAIN_WINFRM_PARTY, &x2, &y2 );

	if( x1 != x2 || y1 != y2 ){
		BOX2OBJ_PartyPokeIconScroll2( syswk );
	}

	if( party_mv == 0 && ret_mv == 0 ){
		ret = FALSE;
	}else{
		ret = TRUE;
	}

	return ret;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���ړ��i�u�|�P��������������v�p�j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"TRUE = �ړ���"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
/*
static BOOL AzukeruPartyPokeFrameMove( BOX2_SYS_WORK * syswk )
{
	u32	party_mv;
	s8	x1, y1, x2, y2;
	BOOL	ret;

	BGWINFRM_PosGet( syswk->app->wfrmwk, BOX2MAIN_WINFRM_PARTY, &x1, &y1 );

	party_mv = BGWINFRM_MoveOne( syswk->app->wfrmwk, BOX2MAIN_WINFRM_PARTY );

	BGWINFRM_PosGet( syswk->app->wfrmwk, BOX2MAIN_WINFRM_PARTY, &x2, &y2 );

	if( x1 != x2 || y1 != y2 ){
		BOX2OBJ_PartyPokeIconScroll2( syswk );
	}

	if( party_mv == 0 ){
		ret = FALSE;
	}else{
		ret = TRUE;
	}

	return ret;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�E�B���h�E�t���[���Ɏ莝���|�P�����t���[���O���t�B�b�N���Z�b�g
 *
 * @param		wk			�a�f�E�B���h�E�t���[�����[�N
 * @param		index		�a�f�E�B���h�E�t���[���C���f�b�N�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PartyPokeFrameLoadArrange( BGWINFRM_WORK * wk, u32 index )
{
	FrameArcLoad( wk, index, NARC_box_gra_box_poke_bg_lz_NSCR );
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

	for( i=0; i<5; i++ ){
		BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MENU1+i, WINFRM_MENU_INIT_PX, WINFRM_MENU_PY+i*WINFRM_MENU_SY );
	}
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_CLOSE_BTN, WINFRM_MODORU_PX, WINFRM_MODORU_PY+BOXPARTY_BTN_CNT );
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
/*
void BOX2MAIN_PokeMenuOpenPosSet( BGWINFRM_WORK * wk )
{
	u32	i;

	for( i=0; i<5; i++ ){
		BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MENU1+i, WINFRM_MENU_PX, WINFRM_MENU_PY+i*WINFRM_MENU_SY );
	}
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_CLOSE_BTN, WINFRM_MODORU_PX, WINFRM_MODORU_PY );
}
*/
//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�{�^����\��
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_PokeMenuOff( BGWINFRM_WORK * wk )
{
	u32	i;

	for( i=0; i<5; i++ ){
		BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_MENU1+i );
		BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MENU1+i, WINFRM_MENU_INIT_PX, WINFRM_MENU_PY+i*WINFRM_MENU_SY );
	}
	BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_CLOSE_BTN );
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_CLOSE_BTN, WINFRM_MODORU_PX, WINFRM_MODORU_PY+BOXPARTY_BTN_CNT );
}
*/
//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_PokeMenuInSet( BGWINFRM_WORK * wk )
{
	u16	i;
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_MENU1, &px, &py );
	if( px == WINFRM_MENU_PX ){ return; }

	for( i=0; i<5; i++ ){
		BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MENU1+i, -1, 0, px-WINFRM_MENU_PX );
	}

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_CLOSE_BTN, &px, &py );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_CLOSE_BTN, 0, -1, py-WINFRM_MODORU_PY );
}
*/
//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_PokeMenuOutSet( BGWINFRM_WORK * wk )
{
	u16	i;
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_MENU1, &px, &py );
	if( px == WINFRM_MENU_INIT_PX ){ return; }

	for( i=0; i<5; i++ ){
		BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MENU1+i, 1, 0, WINFRM_MENU_INIT_PX-px );
	}

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_CLOSE_BTN, &px, &py );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_CLOSE_BTN, 0, 1, WINFRM_MODORU_PY+BOXPARTY_BTN_CNT-py );
}
*/
//--------------------------------------------------------------------------------------------
/**
 * �|�P�������j���[�ړ�
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @retval	"TRUE = �ړ���"
 * @retval	"FLASE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
/*
static BOOL PokeMenuMoveMain( BGWINFRM_WORK * wk )
{
	u32		i;
	BOOL	ret;
	
	ret = FALSE;

	for( i=0; i<5; i++ ){
		if( BGWINFRM_MoveOne( wk, BOX2MAIN_WINFRM_MENU1+i ) == 1 ){
			ret = TRUE;
		}
	}
	if( BGWINFRM_MoveOne( wk, BOX2MAIN_WINFRM_CLOSE_BTN ) == 1 ){
		ret = TRUE;
	}

	return ret;
}
*/
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
/*
BOOL BOX2MAIN_PokeMenuPutCheck( BGWINFRM_WORK * wk )
{
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_MENU1, &px, &py );
	if( px == WINFRM_MENU_INIT_PX ){
		return FALSE;
	}
	return TRUE;
}
*/


//============================================================================================
//	�u�{�b�N�X�����肩����v�{�^��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�����肩����v�{�^������ʊO�֔z�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxMoveButtonInitPut( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_BOXMV_BTN, WINFRM_BOXCHG_BTN_PX, WINFRM_BOXCHG_BTN_INIT_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�����肩����v�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_BoxMoveButtonInSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_BOXMV_BTN, 0, -1, BOXPARTY_BTN_CNT );
}
*/
//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�����肩����v�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_BoxMoveButtonOutSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_BOXMV_BTN, 0, 1, BOXPARTY_BTN_CNT );
}
*/
//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�����肩����v�{�^���z�u�`�F�b�N
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @retval	"TRUE = �z�u�ς�"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
/*
BOOL BOX2MAIN_BoxMoveButtonCheck( BGWINFRM_WORK * wk )
{
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_BOXMV_BTN, &px, &py );
	if( py == WINFRM_BOXCHG_BTN_PY ){
		return TRUE;
	}
	return FALSE;
}
*/

//============================================================================================
//	�x�X�e�[�^�X�{�^��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^������ʊO�֔z�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void YStatusButtonInitPut( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_Y_ST_BTN, WINFRM_Y_ST_BTN_PX, WINFRM_Y_ST_BTN_INIT_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^������ʓ��֔z�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_YStatusButtonPut( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_Y_ST_BTN, WINFRM_Y_ST_BTN_PX, WINFRM_Y_ST_BTN_INIT_PY-BOXPARTY_BTN_CNT );
}
*/
//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_YStatusButtonInSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_Y_ST_BTN, 0, -1, BOXPARTY_BTN_CNT );
}
*/
//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2MAIN_YStatusButtonOutSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_Y_ST_BTN, 0, 1, BOXPARTY_BTN_CNT );
}
*/
//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^���z�u�`�F�b�N
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @retval	"TRUE = �z�u�ς�"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
/*
BOOL BOX2MAIN_YStatusButtonCheck( BGWINFRM_WORK * wk )
{
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_Y_ST_BTN, &px, &py );
	if( py == WINFRM_Y_ST_BTN_PY ){
		return TRUE;
	}
	return FALSE;
}
*/
