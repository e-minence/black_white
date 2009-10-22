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
#define	MARKING_FRM_SY	( 18 )						// �}�[�L���O�t���[���x�T�C�Y
//#define	MARKING_FRM_PX	( 32 - MARKING_FRM_SX )		// �}�[�L���O�t���[���w�\�����W
//#define	MARKING_FRM_PY	( 26 )						// �}�[�L���O�t���[���w�\�����W

#define	MARKING_ON_CHR	( 0x2b )					// �}�[�L���O�L�����J�n�ʒu ( ON )
#define	MARKING_OFF_CHR	( 0x0b )					// �}�[�L���O�L�����J�n�ʒu ( OFF )

// �莝���|�P�����t���[��
#define	PARTYPOKE_FRM_SX	( BOX2BGWFRM_PARTYPOKE_SX )		// �莝���|�P�����t���[���w�T�C�Y
#define	PARTYPOKE_FRM_SY	( 18 )		// �莝���|�P�����t���[���x�T�C�Y
#define	PARTYPOKE_FRM_PX	( 2 )		// �莝���|�P�����t���[���w�\�����W
#define	PARTYPOKE_FRM_PY	( 21 )		// �莝���|�P�����t���[���x�\�����W
#define	WINFRM_PARTYPOKE_LX	( BOX2BGWFRM_PARTYPOKE_LX )
#define	WINFRM_PARTYPOKE_RX	( 21 )
#define	WINFRM_PARTYPOKE_PY	( BOX2BGWFRM_PARTYPOKE_PY )
#define	WINFRM_PARTYPOKE_INIT_PY	( 24 )
#define	WINFRM_PARTYPOKE_RET_PX		( 24 )
#define	WINFRM_PARTYPOKE_RET_PY		( 15 )

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
#define	WINFRM_MENU_PX		( 21 )		// �\���w���W
#define	WINFRM_MENU_PY		( 5 )		// �\���x���W
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
#define	WINFRM_TEMOCHI_OUT_PY	( 24 )
//�u�|�P�������ǂ��v�t���[���f�[�^
#define	WINFRM_IDOU_PX			( 12 )
#define	WINFRM_IDOU_PY			( 21 )
#define	WINFRM_IDOU_OUT_PY		( 24 )
//�u���ǂ�v�t���[���f�[�^
#define	WINFRM_MODORU_PX		( 24 )
#define	WINFRM_MODORU_PY		( 21 )
#define	WINFRM_MODORU_OUT_PY	( 24 )

// �}�[�L���O�z�u�f�[�^
#define	MARK_DEF_PX		( 3 )
#define	MARK_DEF_PY		( 2 )
#define	MARK_DEF_SX		( 4 )
#define	MARK_DEF_SY		( 3 )

// �}�[�L���O�t���[���f�[�^
#define	WINFRM_MARK_PX			( 21 )
#define	WINFRM_MARK_PY			( 6 )
#define	WINFRM_MARK_IN_START_PY	( 24 )
#define	WINFRM_MARK_MV_CNT		( WINFRM_MARK_IN_START_PY - WINFRM_MARK_PY )

// �{�b�N�X�ړ����j���[�t���[���f�[�^
#define	WINFRM_BOXMV_MENU_PX		( 21 )
#define	WINFRM_BOXMV_MENU_PY		( 14 )
#define	WINFRM_BOXMV_MENU_INIT_PX	( 32 )
#define	WINFRM_BOXMV_MENU_SX		( 11 )

// �g���C�I���t���[���f�[�^
#define	WINFRM_TRAYMOVE_IN_PX	( 0 )
#define	WINFRM_TRAYMOVE_IN_PY	( -BOXMOVE_FRM_SY )

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

	BOX2BMP_TemochiButtonBgFrmWkMake( appwk );	//�u�Ă����|�P�����v
	BOX2BMP_IdouButtonBgFrmWkMake( appwk );			//�u�|�P�������ǂ��v
	BOX2BMP_ModoruButtonBgFrmWkMake( appwk );		//�u���ǂ�v�u��߂�v
	BOX2BMP_ToziruButtonBgFrmWkMake( syswk );		//�u�Ƃ���v

	if( syswk->dat->callMode == BOX_MODE_AZUKERU ){
		BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, GFL_BG_FRAME0_M, BOXMV_PTOUT_FRM_SX, BOXMV_PTOUT_FRM_SY );
		FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, NARC_box_gra_box_partyout_bg_lz_NSCR );
	}else{
		BGWINFRM_Add( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, GFL_BG_FRAME1_M, BOXMOVE_FRM_SX, BOXMOVE_FRM_SY );
		FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_MOVE, NARC_box_gra_box_move_bg_lz_NSCR );
	}

	FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_WPCHG, NARC_box_gra_box_wpchg_bg_lz_NSCR );

	if( syswk->dat->callMode == BOX_MODE_AZUKERU || syswk->dat->callMode == BOX_MODE_TURETEIKU ){
		FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_PARTY, NARC_box_gra_box_poke_bg2_lz_NSCR );
	}else{
		BOX2BGWFRM_PartyPokeFrameLoadArrange( appwk->wfrm, BOX2MAIN_WINFRM_PARTY );
		BOX2BMP_PartyCngButtonFrmPut( appwk );
	}

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

	FrameArcLoad( appwk->wfrm, BOX2MAIN_WINFRM_MARK, NARC_box_gra_box_mark_bg_lz_NSCR );
	BOX2BMP_MarkingButtonFrmPut( syswk->app );

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
void BOX2BGWFRM_PartyPokeFrameOff( BOX2_SYS_WORK * syswk )
{
	GFL_BG_FillScreen(
		GFL_BG_FRAME1_M, 0,
		PARTYPOKE_FRM_PX, PARTYPOKE_FRM_PY,
		PARTYPOKE_FRM_SX, PARTYPOKE_FRM_SY, GFL_BG_SCRWRT_PALNL );
}

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
		if( px == WINFRM_PARTYPOKE_LX ){
			BGWINFRM_FramePut(
				wk, BOX2MAIN_WINFRM_RET_BTN, WINFRM_PARTYPOKE_RET_PX, py+WINFRM_PARTYPOKE_RET_PY );
			BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_RET_BTN, 0, -1, py-WINFRM_PARTYPOKE_PY );
		}
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
		if( px == WINFRM_PARTYPOKE_LX ){
			BGWINFRM_FramePut(
				wk, BOX2MAIN_WINFRM_RET_BTN, WINFRM_PARTYPOKE_RET_PX, py+WINFRM_PARTYPOKE_RET_PY );
			BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_RET_BTN, 0, 1, WINFRM_PARTYPOKE_INIT_PY-py );
		}
	}
}

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
void BOX2BGWFRM_PartyPokeFrameOnlyOutSet( BGWINFRM_WORK * wk )
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
 * �莝���|�P�����̂݃t���[���C���Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 *
 * @li	�ʏ�́u��߂�v���Z�b�g�����
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_PartyPokeFrameOnlyInSet( BGWINFRM_WORK * wk )
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
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_RET_BTN, 1, 0, BOX2MAIN_PARTYPOKE_FRM_H_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[���E�ړ��Z�b�g�i�u�|�P��������������v�p�j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_AzukeruPartyPokeFrameRightMoveSet( BGWINFRM_WORK * wk )
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

	BGWINFRM_FramePut(
		wk, BOX2MAIN_WINFRM_RET_BTN,
		WINFRM_PARTYPOKE_RET_PX+BOX2MAIN_PARTYPOKE_FRM_H_CNT,
		WINFRM_PARTYPOKE_PY+WINFRM_PARTYPOKE_RET_PY );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_RET_BTN, -1, 0, BOX2MAIN_PARTYPOKE_FRM_H_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����t���[�����ړ��Z�b�g�i�u�|�P��������������v�p�j
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_AzukeruPartyPokeFrameLeftMoveSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_PARTY, -1, 0, BOX2MAIN_PARTYPOKE_FRM_H_CNT );
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
	u32	party_mv, ret_mv;
	s8	x1, y1, x2, y2;
	BOOL	ret;

	BGWINFRM_PosGet( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY, &x1, &y1 );

	party_mv = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY );
	ret_mv   = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_RET_BTN );
	BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_Y_ST_BTN );

	BGWINFRM_PosGet( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY, &x2, &y2 );

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
BOOL BOX2BGWFRM_AzukeruPartyPokeFrameMove( BOX2_SYS_WORK * syswk )
{
	u32	party_mv;
	s8	x1, y1, x2, y2;
	BOOL	ret;

	BGWINFRM_PosGet( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY, &x1, &y1 );

	party_mv = BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY );

	BGWINFRM_PosGet( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY, &x2, &y2 );

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
void BOX2BGWFRM_PokeMenuOpenPosSet( BGWINFRM_WORK * wk )
{
	u32	i;

	for( i=0; i<5; i++ ){
		BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MENU1+i, WINFRM_MENU_PX, WINFRM_MENU_PY+i*WINFRM_MENU_SY );
	}
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_CLOSE_BTN, WINFRM_MODORU_PX, WINFRM_MODORU_PY );
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

	for( i=0; i<5; i++ ){
		BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_MENU1+i );
		BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MENU1+i, WINFRM_MENU_INIT_PX, WINFRM_MENU_PY+i*WINFRM_MENU_SY );
	}
	BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_CLOSE_BTN );
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_CLOSE_BTN, WINFRM_MODORU_PX, WINFRM_MODORU_PY+BOXPARTY_BTN_CNT );
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

	for( i=0; i<5; i++ ){
		BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MENU1+i, -1, 0, px-WINFRM_MENU_PX );
	}

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_CLOSE_BTN, &px, &py );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_CLOSE_BTN, 0, -1, py-WINFRM_MODORU_PY );
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

	for( i=0; i<5; i++ ){
		BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MENU1+i, 1, 0, WINFRM_MENU_INIT_PX-px );
	}

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_CLOSE_BTN, &px, &py );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_CLOSE_BTN, 0, 1, WINFRM_MODORU_PY+BOXPARTY_BTN_CNT-py );
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
void BOX2BGWFRM_BoxMoveButtonInSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_BOXMV_BTN, 0, -1, BOXPARTY_BTN_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�����肩����v�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_BoxMoveButtonOutSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_BOXMV_BTN, 0, 1, BOXPARTY_BTN_CNT );
}

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
BOOL BOX2BGWFRM_BoxMoveButtonCheck( BGWINFRM_WORK * wk )
{
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_BOXMV_BTN, &px, &py );
	if( py == WINFRM_BOXCHG_BTN_PY ){
		return TRUE;
	}
	return FALSE;
}


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
void BOX2BGWFRM_YStatusButtonPut( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_Y_ST_BTN, WINFRM_Y_ST_BTN_PX, WINFRM_Y_ST_BTN_INIT_PY-BOXPARTY_BTN_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_YStatusButtonInSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_Y_ST_BTN, 0, -1, BOXPARTY_BTN_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_YStatusButtonOutSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_Y_ST_BTN, 0, 1, BOXPARTY_BTN_CNT );
}

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
BOOL BOX2BGWFRM_YStatusButtonCheck( BGWINFRM_WORK * wk )
{
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_Y_ST_BTN, &px, &py );
	if( py == WINFRM_Y_ST_BTN_PY ){
		return TRUE;
	}
	return FALSE;
}


//============================================================================================
//	���̑�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�Ă����|�P�����{�^���v�{�^���z�u
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_ButtonPutTemochi( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_ButtonPutTemochi( syswk );
	BGWINFRM_FramePut( syswk->app->wfrm, BOX2MAIN_WINFRM_POKE_BTN, WINFRM_TEMOCHI_PX, WINFRM_TEMOCHI_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�Ă����|�P�����{�^���v�{�^������ʊO�ɔz�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_ButtonOutPutTemochi( BOX2_SYS_WORK * syswk )
{
	BGWINFRM_FramePut( syswk->app->wfrm, BOX2MAIN_WINFRM_POKE_BTN, WINFRM_TEMOCHI_PX, WINFRM_TEMOCHI_OUT_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�Ă����|�P�����{�^���v�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_TemochiButtonOutSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_POKE_BTN, 0, 1, BOXPARTY_BTN_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�Ă����|�P�����{�^���v�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_TemochiButtonInSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_POKE_BTN, 0, -1, BOXPARTY_BTN_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�|�P�������ǂ��v�{�^���z�u
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_ButtonPutIdou( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_ButtonPutIdou( syswk );
	BGWINFRM_FramePut( syswk->app->wfrm, BOX2MAIN_WINFRM_MV_BTN, WINFRM_IDOU_PX, WINFRM_IDOU_PY );
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
void BOX2BGWFRM_ButtonOutPutIdou( BOX2_SYS_WORK * syswk )
{
	BGWINFRM_FramePut( syswk->app->wfrm, BOX2MAIN_WINFRM_MV_BTN, WINFRM_IDOU_PX, WINFRM_IDOU_OUT_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P�������ǂ��v�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_IdouButtonOutSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MV_BTN, 0, 1, BOXPARTY_BTN_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P�������ǂ��v�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_IdouButtonInSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MV_BTN, 0, -1, BOXPARTY_BTN_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u���ǂ�v�{�^���z�u
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_ButtonPutModoru( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_ButtonPutModoru( syswk );
	BGWINFRM_FramePut( syswk->app->wfrm, BOX2MAIN_WINFRM_RET_BTN, WINFRM_MODORU_PX, WINFRM_MODORU_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �u��߂�v�{�^���z�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_ButtonPutYameru( BOX2_SYS_WORK * syswk )
{
	BOX2BMP_ButtonPutYameru( syswk );
	BGWINFRM_FramePut( syswk->app->wfrm, BOX2MAIN_WINFRM_RET_BTN, WINFRM_MODORU_PX, WINFRM_MODORU_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �L�����Z���{�^������ʊO�ɔz�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_RetButtonOutPut( BGWINFRM_WORK * wk )
{
	BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_RET_BTN );
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_RET_BTN, WINFRM_MODORU_PX, WINFRM_MODORU_OUT_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �L�����Z���{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_RetButtonOutSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_RET_BTN, 0, 1, BOXPARTY_BTN_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �L�����Z���{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_RetButtonInSet( BGWINFRM_WORK * wk )
{
	// �莝���t���[������ňʒu���ύX����Ă���\��������̂ŁA�Đݒ�
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_RET_BTN, WINFRM_MODORU_PX, WINFRM_MODORU_OUT_PY );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_RET_BTN, 0, -1, BOXPARTY_BTN_CNT );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�Ƃ���v�{�^���z�u�`�F�b�N
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @retval	"TRUE = �z�u�ς�"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2BGWFRM_CloseButtonPutCheck( BGWINFRM_WORK * wk )
{
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_CLOSE_BTN, &px, &py );
	if( py == WINFRM_MODORU_PY ){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �u�Ă����|�P�����v�u�|�P�������ǂ��v�{�^����\��
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_BoxPartyButtonVanish( BGWINFRM_WORK * wk )
{
	BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_POKE_BTN );
	BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_MV_BTN );

	BGWINFRM_FramePut(
		wk, BOX2MAIN_WINFRM_POKE_BTN,
		WINFRM_TEMOCHI_PX, WINFRM_TEMOCHI_PY+BOXPARTY_BTN_CNT );
	BGWINFRM_FramePut(
		wk, BOX2MAIN_WINFRM_MV_BTN,
		WINFRM_IDOU_PX, WINFRM_IDOU_PY+BOXPARTY_BTN_CNT );
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
void BOX2BGWFRM_SubDispWazaFrmInSet( BOX2_APP_WORK * appwk )
{
	if( appwk->subdisp_waza_put == 0 ){ return; }
	BGWINFRM_MoveInit( appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP, -1, 0, SUBDISP_WAZA_FRM_SX );
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʋZ�t���[���A�E�g�Z�b�g
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_SubDispWazaFrmOutSet( BOX2_APP_WORK * appwk )
{
	if( appwk->subdisp_waza_put == 0 ){ return; }
	BGWINFRM_MoveInit( appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP, 1, 0, SUBDISP_WAZA_FRM_SX );
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʋZ�t���[����ʓ��ɔz�u
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_SubDispWazaFrmInPosSet( BOX2_APP_WORK * appwk )
{
	if( appwk->subdisp_waza_put == 0 ){ return; }
	BGWINFRM_FrameOff( appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP );
	BGWINFRM_FramePut(
		appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP,
		SUBDISP_WAZA_FRM_IN_PX-SUBDISP_WAZA_FRM_SX, SUBDISP_WAZA_FRM_IN_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʋZ�t���[����ʊO�ɔz�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_SubDispWazaFrmOutPosSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_SUBDISP );
	BGWINFRM_FramePut(
		wk, BOX2MAIN_WINFRM_SUBDISP, SUBDISP_WAZA_FRM_IN_PX, SUBDISP_WAZA_FRM_IN_PY );
}


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
void BOX2BGWFRM_SubDispItemFrmInSet( BGWINFRM_WORK * wk )
{
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_SUBDISP, &px, &py );
	if( py == SUBDISP_ITEM_FRM_IN_PY-SUBDISP_ITEM_FRM_SY ){
		return;
	}
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_SUBDISP, 0, -1, SUBDISP_ITEM_FRM_SY-(SUBDISP_ITEM_FRM_IN_PY-py) );
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʎ������t���[���A�E�g�Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_SubDispItemFrmOutSet( BGWINFRM_WORK * wk )
{
	s8	px, py;

	BGWINFRM_PosGet( wk, BOX2MAIN_WINFRM_SUBDISP, &px, &py );
	if( py == SUBDISP_ITEM_FRM_IN_PY ){
		return;
	}
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_SUBDISP, 0, 1, SUBDISP_ITEM_FRM_IN_PY-py );
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʎ������t���[������ʓ��ɔz�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_SubDispItemFrmInPosSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_SUBDISP );
	BGWINFRM_FramePut(
		wk, BOX2MAIN_WINFRM_SUBDISP, SUBDISP_ITEM_FRM_IN_PX, SUBDISP_ITEM_FRM_IN_PY-SUBDISP_ITEM_FRM_SY );
}

//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʎ������t���[������ʊO�ɔz�u
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_SubDispItemFrmOutPosSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FrameOff( wk, BOX2MAIN_WINFRM_SUBDISP );
	BGWINFRM_FramePut(
		wk, BOX2MAIN_WINFRM_SUBDISP, SUBDISP_ITEM_FRM_IN_PX, SUBDISP_ITEM_FRM_IN_PY );
}


//============================================================================================
//	�}�[�L���O�t���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �}�[�N�z�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�}�[�N�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MarkingScrPut( BOX2_SYS_WORK * syswk, u32 pos )
{
	u16 * scr;
	u16	chr;
	u8	x, y;

	x = MARK_DEF_PX + MARK_DEF_SX * ( pos & 1 );
	y = MARK_DEF_PY + MARK_DEF_SY * ( pos / 2 );

	scr = BGWINFRM_FrameBufGet( syswk->app->wfrm, BOX2MAIN_WINFRM_MARK ) + MARKING_FRM_SX * y + x;

	if( ( syswk->app->pokeMark & (1<<pos) ) == 0 ){
		chr = MARKING_OFF_CHR + pos;
	}else{
		chr = MARKING_ON_CHR + pos;
	}

	*scr = ( *scr & 0xf000 ) + chr;
}

//--------------------------------------------------------------------------------------------
/**
 * �}�[�N�L���O�t���[���\��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_MarkingFramePut( BOX2_SYS_WORK * syswk )
{
	u32	i;

	for( i=0; i<POKEPARA_MARKING_ELEMS_MAX; i++ ){
		MarkingScrPut( syswk, i );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �}�[�N�\���؂�ւ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�}�[�N�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_MarkingSwitch( BOX2_SYS_WORK * syswk, u32 pos )
{
	syswk->app->pokeMark ^= (1<<pos);
	MarkingScrPut( syswk, pos );
	BGWINFRM_FramePut( syswk->app->wfrm, BOX2MAIN_WINFRM_MARK, WINFRM_MARK_PX, WINFRM_MARK_PY );
}

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


//============================================================================================
//	�ǎ��ύX�t���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX�t���[���C���Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_WallPaperFrameInSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_WPCHG, WINFRM_WALLPAPER_PX, WINFRM_WALLPAPER_IN_PY );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_WPCHG, 0, 1, WPCHG_FRM_SY );
}

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX�t���[���A�E�g�Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_WallPaperFrameOutSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_WPCHG, WINFRM_WALLPAPER_PX, WINFRM_WALLPAPER_OUT_PY );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_WPCHG, 0, -1, WPCHG_FRM_SY );
}


//============================================================================================
//	�{�b�N�X�؂�ւ����j���[
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�؂�ւ����j���[�{�^����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 *
 * @li	�ǎ��ύX�A�a����{�b�N�X�I���Ŏg�p���Ă܂�
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_BoxMoveMenuInSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_BOXMV_MENU, WINFRM_BOXMV_MENU_INIT_PX, WINFRM_BOXMV_MENU_PY );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_BOXMV_MENU, -1, 0, WINFRM_BOXMV_MENU_SX );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�؂�ւ����j���[�{�^����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 *
 * @li	�ǎ��ύX�A�a����{�b�N�X�I���Ŏg�p���Ă܂�
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_BoxMoveMenuOutSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_BOXMV_MENU, WINFRM_BOXMV_MENU_PX, WINFRM_BOXMV_MENU_PY );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_BOXMV_MENU, 1, 0, WINFRM_BOXMV_MENU_SX );
}


//============================================================================================
//	�g���C�I���t���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �g���C�I���t���[������ʓ��֔z�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 *
 *	����ʂ��畜�A���Ƀ{�b�N�X�ړ��t���[����\��
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_BoxMoveFrmPut( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_ButtonPutYameru( syswk );

	BGWINFRM_FrameOff( syswk->app->wfrm, BOX2MAIN_WINFRM_POKE_BTN );
	BGWINFRM_FrameOff( syswk->app->wfrm, BOX2MAIN_WINFRM_MV_BTN );
	BGWINFRM_FramePut( syswk->app->wfrm, BOX2MAIN_WINFRM_POKE_BTN, WINFRM_TEMOCHI_PX, WINFRM_TEMOCHI_OUT_PY );
	BGWINFRM_FramePut( syswk->app->wfrm, BOX2MAIN_WINFRM_MV_BTN, WINFRM_IDOU_PX, WINFRM_IDOU_OUT_PY );

	BGWINFRM_FramePut( syswk->app->wfrm, BOX2MAIN_WINFRM_MOVE, 0, 0 );

	BOX2BMP_BoxMoveNameNumWrite( syswk );
	BOX2OBJ_BoxMoveObjInit( syswk );
	BOX2OBJ_BoxMoveCursorSet( syswk );
	BOX2OBJ_BoxMoveNameSet( syswk );
	BOX2OBJ_BoxMoveFrmScroll( syswk, 8 * BOXMOVE_FRM_SY );
}

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
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MOVE, 0, 1, BOXMOVE_FRM_SY );
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
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MOVE, 0, -1, BOXMOVE_FRM_SY );
}


//============================================================================================
// �{�b�N�X�e�[�}�ύX�t���[���\��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�e�[�}�ύX���j���[�\��
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxThemaMenuOn( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_BOXMV_MENU, WINFRM_BOXTHEMA_MENU_PX, WINFRM_BOXTHEMA_MENU1_PY );
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MENU4, WINFRM_BOXTHEMA_MENU_PX, WINFRM_BOXTHEMA_MENU2_PY );
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MENU5, WINFRM_BOXTHEMA_MENU_PX, WINFRM_BOXTHEMA_MENU3_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�e�[�}�ύX�t���[������ʓ��֔z�u
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 *
 *	����ʂ��畜�A���Ƀ{�b�N�X�e�[�}�ύX�֘A�t���[����\��
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_BoxThemaFrmPut( BOX2_SYS_WORK * syswk )
{
	BOX2BGWFRM_BoxMoveFrmPut( syswk );

	BOX2BMP_BoxThemaMenuPrint( syswk );
	BoxThemaMenuOn( syswk->app->wfrm );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�e�[�}�ύX�t���[����ʓ��ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_BoxThemaMenuInSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_BOXMV_MENU, WINFRM_BOXTHEMA_MENU_OUT_PX, WINFRM_BOXTHEMA_MENU1_PY );
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MENU4, WINFRM_BOXTHEMA_MENU_OUT_PX, WINFRM_BOXTHEMA_MENU2_PY );
	BGWINFRM_FramePut( wk, BOX2MAIN_WINFRM_MENU5, WINFRM_BOXTHEMA_MENU_OUT_PX, WINFRM_BOXTHEMA_MENU3_PY );

	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_BOXMV_MENU, -1, 0, WINFRM_BOXTHEMA_MENU_SX );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MENU4, -1, 0, WINFRM_BOXTHEMA_MENU_SX );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MENU5, -1, 0, WINFRM_BOXTHEMA_MENU_SX );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�e�[�}�ύX�t���[����ʊO�ւ̈ړ��Z�b�g
 *
 * @param	wk		�a�f�E�B���h�E�t���[�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_BoxThemaMenuOutSet( BGWINFRM_WORK * wk )
{
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_BOXMV_MENU, 1, 0, WINFRM_BOXTHEMA_MENU_SX );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MENU4, 1, 0, WINFRM_BOXTHEMA_MENU_SX );
	BGWINFRM_MoveInit( wk, BOX2MAIN_WINFRM_MENU5, 1, 0, WINFRM_BOXTHEMA_MENU_SX );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�Ă����|�P�����v�u�|�P�������ǂ��v���\���ɂ���
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_ArrangeUnderButtonDel( BOX2_SYS_WORK * syswk )
{
	BGWINFRM_FrameOff( syswk->app->wfrm, BOX2MAIN_WINFRM_POKE_BTN );
	BGWINFRM_FrameOff( syswk->app->wfrm, BOX2MAIN_WINFRM_MV_BTN );

	BOX2BGWFRM_ButtonOutPutTemochi( syswk );
	BOX2BGWFRM_ButtonOutPutIdou( syswk );
}


//--------------------------------------------------------------------------------------------
/**
 * �T�u��ʃE�B���h�E����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2BGWFRM_SubDispWinFrmMove( BOX2_SYS_WORK * syswk )
{
	if( syswk->dat->callMode != BOX_MODE_ITEM ){
		BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_SUBDISP );
	}else{
		BGWINFRM_MoveOne( syswk->app->wfrm, BOX2MAIN_WINFRM_SUBDISP );
		BOX2OBJ_ItemIconPutSub( syswk->app );
	}
}
