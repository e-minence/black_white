//============================================================================================
/**
 * @file		b_bag_anm.c
 * @brief		�퓬�p�o�b�O��� �{�^������
 * @author	Hiroyuki Nakamura
 * @date		09.08.26
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/palanm.h"

#include "b_app_tool.h"

#include "b_bag.h"
#include "b_bag_main.h"
#include "b_bag_item.h"
#include "b_bag_anm.h"
#include "b_bag_gra.naix"


//============================================================================================
//	�萔��`
//============================================================================================
// �{�^���A�j���p�^�[��
enum {
	BBAG_BANM_PAT0 = 0,	// �ʏ�
	BBAG_BANM_PAT1,			// �A�j���P
	BBAG_BANM_PAT2,			// �A�j���Q
	BBAG_BANM_NONE,			// �����Ȃ�
};

// �P�y�[�W�ڂ̃{�^�����W
#define	P1_PAGE1_SCR_PX		( 0 )
#define	P1_PAGE1_SCR_PY		( 1 )
#define	P1_PAGE2_SCR_PX		( 0 )
#define	P1_PAGE2_SCR_PY		( P1_PAGE1_SCR_PY + BBAG_BSY_POCKET )
#define	P1_PAGE3_SCR_PX		( BBAG_BSX_POCKET )
#define	P1_PAGE3_SCR_PY		( 1 )
#define	P1_PAGE4_SCR_PX		( BBAG_BSX_POCKET )
#define	P1_PAGE4_SCR_PY		( P1_PAGE3_SCR_PY + BBAG_BSY_POCKET )
#define	P1_LASTITEM_SCR_PX	( 1 )
#define	P1_LASTITEM_SCR_PY	( 19 )
#define	P1_RETURN_SCR_PX	( 27 )
#define	P1_RETURN_SCR_PY	( 19 )
// �Q�y�[�W�ڂ̃{�^�����W
#define	P2_ITEM1_SCR_PX		( 32 )
#define	P2_ITEM1_SCR_PY		( 1 )
#define	P2_ITEM2_SCR_PX		( 48 )
#define	P2_ITEM2_SCR_PY		( 1 )
#define	P2_ITEM3_SCR_PX		( 32 )
#define	P2_ITEM3_SCR_PY		( 7 )
#define	P2_ITEM4_SCR_PX		( 48 )
#define	P2_ITEM4_SCR_PY		( 7 )
#define	P2_ITEM5_SCR_PX		( 32 )
#define	P2_ITEM5_SCR_PY		( 13 )
#define	P2_ITEM6_SCR_PX		( 48 )
#define	P2_ITEM6_SCR_PY		( 13 )
#define	P2_UP_SCR_PX		( 32 )
#define	P2_UP_SCR_PY		( 19 )
#define	P2_DOWN_SCR_PX		( 37 )
#define	P2_DOWN_SCR_PY		( 19 )
#define	P2_RETURN_SCR_PX	( 59 )
#define	P2_RETURN_SCR_PY	( 19 )
// �R�y�[�W�ڂ̃{�^�����W
#define	P3_USE_SCR_PX		( 1 )
#define	P3_USE_SCR_PY		( 51 )
#define	P3_RETURN_SCR_PX	( 27 )
#define	P3_RETURN_SCR_PY	( 51 )

// �{�^���t���[���f�[�^
typedef struct {
	u16	arc;
	u8	sx;
	u8	sy;
}BGWF_DATA;

#define	BUTTON_PAL_NORMAL		( 4 )
#define	BUTTON_PAL_ON				( 5 )
#define	BUTTON_PAL_OFF			( 6 )

#define	SYSBTN_PAL_NORMAL		( 1 )
#define	SYSBTN_PAL_ON				( 2 )
#define	SYSBTN_PAL_OFF			( 3 )

#define	SWAP_BMP_POS	( BBAG_BTNANM_RET3+1 )		// �X���b�v�p�f�[�^�ʒu


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//============================================================================================
//	�O���[�o��
//============================================================================================

// �{�^�����BMP
static const u8 BtnBmpWin_Page1[] = { WIN_P1_HP, 0xff };		// �uPP�����ӂ��v
static const u8 BtnBmpWin_Page2[] = { WIN_P1_ZYOUTAI, 0xff };	// �u���傤���������ӂ��v
static const u8 BtnBmpWin_Page3[] = { WIN_P1_BALL, 0xff };		// �u�{�[���v
static const u8 BtnBmpWin_Page4[] = { WIN_P1_BATTLE, 0xff };	// �u����Ƃ��悤�v
static const u8 BtnBmpWin_Last[]  = { WIN_P1_LASTITEM, 0xff };	// �u�������ɂ������ǂ����v
static const u8 BtnBmpWin_Item1[] = { WIN_P2_NAME1, WIN_P2_NUM1, 0xff };
static const u8 BtnBmpWin_Item2[] = { WIN_P2_NAME2, WIN_P2_NUM2, 0xff };
static const u8 BtnBmpWin_Item3[] = { WIN_P2_NAME3, WIN_P2_NUM3, 0xff };
static const u8 BtnBmpWin_Item4[] = { WIN_P2_NAME4, WIN_P2_NUM4, 0xff };
static const u8 BtnBmpWin_Item5[] = { WIN_P2_NAME5, WIN_P2_NUM5, 0xff };
static const u8 BtnBmpWin_Item6[] = { WIN_P2_NAME6, WIN_P2_NUM6, 0xff };
static const u8 BtnBmpWin_Item1_S[] = { WIN_P2_NAME1_S, WIN_P2_NUM1_S, 0xff };
static const u8 BtnBmpWin_Item2_S[] = { WIN_P2_NAME2_S, WIN_P2_NUM2_S, 0xff };
static const u8 BtnBmpWin_Item3_S[] = { WIN_P2_NAME3_S, WIN_P2_NUM3_S, 0xff };
static const u8 BtnBmpWin_Item4_S[] = { WIN_P2_NAME4_S, WIN_P2_NUM4_S, 0xff };
static const u8 BtnBmpWin_Item5_S[] = { WIN_P2_NAME5_S, WIN_P2_NUM5_S, 0xff };
static const u8 BtnBmpWin_Item6_S[] = { WIN_P2_NAME6_S, WIN_P2_NUM6_S, 0xff };
static const u8 BtnBmpWin_Use[] = { WIN_P3_USE, 0xff };

// �{�^�����BMP�f�[�^�e�[�u��
static const u8 * const ButtonBmpWinIndex[] = {
	BtnBmpWin_Page1,	// �uHP�����ӂ��v
	BtnBmpWin_Page2,	// �u���傤���������ӂ��v
	BtnBmpWin_Page3,	// �u�{�[���v
	BtnBmpWin_Page4,	// �u����Ƃ��悤�v
	BtnBmpWin_Last,		// �u�������ɂ������ǂ����v
	NULL,				// �߂�

	BtnBmpWin_Item1,
	BtnBmpWin_Item2,
	BtnBmpWin_Item3,
	BtnBmpWin_Item4,
	BtnBmpWin_Item5,
	BtnBmpWin_Item6,
	NULL,				// �O��
	NULL,				// ����
	NULL,				// �߂�

	BtnBmpWin_Use,		// �u�����v
	NULL,

	BtnBmpWin_Item1_S,
	BtnBmpWin_Item2_S,
	BtnBmpWin_Item3_S,
	BtnBmpWin_Item4_S,
	BtnBmpWin_Item5_S,
	BtnBmpWin_Item6_S,
};

// �{�^���t���[���f�[�^
static const BGWF_DATA ButtonAddData[] =
{
	{ NARC_b_bag_gra_pocket_button01_lz_NSCR, 16, 8 },
	{ NARC_b_bag_gra_pocket_button11_lz_NSCR, 16, 8 },
	{ NARC_b_bag_gra_pocket_button21_lz_NSCR, 16, 8 },
	{ NARC_b_bag_gra_pocket_button31_lz_NSCR, 16, 8 },

	{ NARC_b_bag_gra_item_button01_lz_NSCR, 16, 6 },
	{ NARC_b_bag_gra_item_button01_lz_NSCR, 16, 6 },
	{ NARC_b_bag_gra_item_button01_lz_NSCR, 16, 6 },
	{ NARC_b_bag_gra_item_button01_lz_NSCR, 16, 6 },
	{ NARC_b_bag_gra_item_button01_lz_NSCR, 16, 6 },
	{ NARC_b_bag_gra_item_button01_lz_NSCR, 16, 6 },

	{ NARC_b_bag_gra_return_button01_lz_NSCR, 5, 5 },
	{ NARC_b_bag_gra_left_button01_lz_NSCR, 5, 5 },
	{ NARC_b_bag_gra_right_button01_lz_NSCR, 5, 5 },
	{ NARC_b_bag_gra_use_button01_lz_NSCR, 25, 5 },
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�^���쐬
 *
 * @param		wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BBAGANM_ButtonInit( BBAG_WORK * wk )
{
	ARCHANDLE * ah;
	u32	i;

	wk->bgwfrm = BGWINFRM_Create( BGWINFRM_TRANS_VBLANK, BBAG_BGWF_MAX, wk->dat->heap );

	ah = GFL_ARC_OpenDataHandle( ARCID_B_BAG_GRA, GFL_HEAP_LOWID(wk->dat->heap) );

	for( i=0; i<BBAG_BGWF_MAX; i++ ){
		BGWINFRM_Add( wk->bgwfrm, i, GFL_BG_FRAME2_S, ButtonAddData[i].sx, ButtonAddData[i].sy );
		BGWINFRM_PutAreaSet( wk->bgwfrm, i, 0, 64, 0, 64 );
		BGWINFRM_FrameSetArcHandle( wk->bgwfrm, i, ah, ButtonAddData[i].arc, TRUE );
	}

	GFL_ARC_CloseDataHandle( ah );

	// �ʒu�Œ�̃{�^���������W��ݒ肵�Ă���
	BGWINFRM_FramePut( wk->bgwfrm, BBAG_BGWF_POCKET01, P1_PAGE1_SCR_PX, P1_PAGE1_SCR_PY );
	BGWINFRM_FramePut( wk->bgwfrm, BBAG_BGWF_POCKET02, P1_PAGE2_SCR_PX, P1_PAGE2_SCR_PY );
	BGWINFRM_FramePut( wk->bgwfrm, BBAG_BGWF_POCKET03, P1_PAGE3_SCR_PX, P1_PAGE3_SCR_PY );
	BGWINFRM_FramePut( wk->bgwfrm, BBAG_BGWF_POCKET04, P1_PAGE4_SCR_PX, P1_PAGE4_SCR_PY );

	BGWINFRM_FramePut( wk->bgwfrm, BBAG_BGWF_ITEM01, P2_ITEM1_SCR_PX, P2_ITEM1_SCR_PY );
	BGWINFRM_FramePut( wk->bgwfrm, BBAG_BGWF_ITEM02, P2_ITEM2_SCR_PX, P2_ITEM2_SCR_PY );
	BGWINFRM_FramePut( wk->bgwfrm, BBAG_BGWF_ITEM03, P2_ITEM3_SCR_PX, P2_ITEM3_SCR_PY );
	BGWINFRM_FramePut( wk->bgwfrm, BBAG_BGWF_ITEM04, P2_ITEM4_SCR_PX, P2_ITEM4_SCR_PY );
	BGWINFRM_FramePut( wk->bgwfrm, BBAG_BGWF_ITEM05, P2_ITEM5_SCR_PX, P2_ITEM5_SCR_PY );
	BGWINFRM_FramePut( wk->bgwfrm, BBAG_BGWF_ITEM06, P2_ITEM6_SCR_PX, P2_ITEM6_SCR_PY );

	BGWINFRM_FramePut( wk->bgwfrm, BBAG_BGWF_LEFT, P2_UP_SCR_PX, P2_UP_SCR_PY );
	BGWINFRM_FramePut( wk->bgwfrm, BBAG_BGWF_RIGHT, P2_DOWN_SCR_PX, P2_DOWN_SCR_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�^���폜
 *
 * @param		wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BBAGANM_ButtonExit( BBAG_WORK * wk )
{
	BGWINFRM_Exit( wk->bgwfrm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�^��BG������
 *
 * @param		wk		�퓬�o�b�O�̃��[�N
 * @param		page	�y�[�W�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BBAGANM_PageButtonPut( BBAG_WORK * wk, u8 page )
{
	u32	i;

	for( i=0; i<BBAG_BGWF_MAX; i++ ){
		BGWINFRM_FrameOff( wk->bgwfrm, i );
	}

	switch( page ){
	case BBAG_PAGE_POCKET:	// �|�P�b�g�I���y�[�W
		BGWINFRM_FramePut( wk->bgwfrm,BBAG_BGWF_RETURN, P1_RETURN_SCR_PX, P1_RETURN_SCR_PY );
		BGWINFRM_FramePut( wk->bgwfrm,BBAG_BGWF_USE, P1_LASTITEM_SCR_PX, P1_LASTITEM_SCR_PY );
		if( wk->used_item == ITEM_DUMMY_DATA ){
			BGWINFRM_PaletteChange(
				wk->bgwfrm, BBAG_BGWF_USE, 0, 0,
				ButtonAddData[BBAG_BGWF_USE].sx, ButtonAddData[BBAG_BGWF_USE].sy, SYSBTN_PAL_OFF );
		}else{
			BGWINFRM_PaletteChange(
				wk->bgwfrm, BBAG_BGWF_USE, 0, 0,
				ButtonAddData[BBAG_BGWF_USE].sx, ButtonAddData[BBAG_BGWF_USE].sy, SYSBTN_PAL_NORMAL );
		}
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_POCKET01 );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_POCKET02 );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_POCKET03 );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_POCKET04 );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_RETURN );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_USE );
		break;

	case BBAG_PAGE_MAIN:	// �A�C�e���I���y�[�W
		BGWINFRM_FramePut( wk->bgwfrm,BBAG_BGWF_RETURN, P2_RETURN_SCR_PX, P2_RETURN_SCR_PY );
		for( i=0; i<6; i++ ){
			if( BattleBag_PosItemCheck( wk, i ) == 0 ){
				BGWINFRM_PaletteChange(
					wk->bgwfrm, BBAG_BGWF_ITEM01+i, 0, 0,
					ButtonAddData[BBAG_BGWF_ITEM01+i].sx, ButtonAddData[BBAG_BGWF_ITEM01+i].sy, BUTTON_PAL_OFF );
			}else{
				BGWINFRM_PaletteChange(
					wk->bgwfrm, BBAG_BGWF_ITEM01+i, 0, 0,
					ButtonAddData[BBAG_BGWF_ITEM01+i].sx, ButtonAddData[BBAG_BGWF_ITEM01+i].sy, BUTTON_PAL_NORMAL );
			}
		}
		if( wk->scr_max[wk->poke_id] == 0 ){
				BGWINFRM_PaletteChange(
					wk->bgwfrm, BBAG_BGWF_LEFT, 0, 0,
					ButtonAddData[BBAG_BGWF_LEFT].sx, ButtonAddData[BBAG_BGWF_LEFT].sy, SYSBTN_PAL_OFF );
				BGWINFRM_PaletteChange(
					wk->bgwfrm, BBAG_BGWF_RIGHT, 0, 0,
					ButtonAddData[BBAG_BGWF_RIGHT].sx, ButtonAddData[BBAG_BGWF_RIGHT].sy, SYSBTN_PAL_OFF );
		}else{
				BGWINFRM_PaletteChange(
					wk->bgwfrm, BBAG_BGWF_LEFT, 0, 0,
					ButtonAddData[BBAG_BGWF_LEFT].sx, ButtonAddData[BBAG_BGWF_LEFT].sy, SYSBTN_PAL_NORMAL );
				BGWINFRM_PaletteChange(
					wk->bgwfrm, BBAG_BGWF_RIGHT, 0, 0,
					ButtonAddData[BBAG_BGWF_RIGHT].sx, ButtonAddData[BBAG_BGWF_RIGHT].sy, SYSBTN_PAL_NORMAL );
		}
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_ITEM01 );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_ITEM02 );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_ITEM03 );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_ITEM04 );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_ITEM05 );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_ITEM06 );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_LEFT );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_RIGHT );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_RETURN );
		break;

	case BBAG_PAGE_ITEM:	// �A�C�e���g�p�y�[�W
		BGWINFRM_PaletteChange(
			wk->bgwfrm, BBAG_BGWF_USE, 0, 0,
			ButtonAddData[BBAG_BGWF_USE].sx, ButtonAddData[BBAG_BGWF_USE].sy, SYSBTN_PAL_NORMAL );
		BGWINFRM_FramePut( wk->bgwfrm,BBAG_BGWF_RETURN, P3_RETURN_SCR_PX, P3_RETURN_SCR_PY );
		BGWINFRM_FramePut( wk->bgwfrm,BBAG_BGWF_USE, P3_USE_SCR_PX, P3_USE_SCR_PY );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_RETURN );
		BGWINFRM_FrameOn( wk->bgwfrm, BBAG_BGWF_USE );
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�^���A�j��������
 *
 * @param		wk		�퓬�o�b�O�̃��[�N
 * @param		id		�{�^��ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BBAGANM_ButtonAnmInit( BBAG_WORK * wk, u8 id )
{
	wk->btn_seq  = 0;
	wk->btn_cnt  = 0;
	wk->btn_id   = id;
	wk->btn_flg  = 1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�^���A�j�����C��
 *
 * @param		wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BBAGANM_ButtonAnmMain( BBAG_WORK * wk )
{
	if( wk->btn_flg != 0 ){
		u8	pal;

		switch( wk->btn_seq ){
		case 0:
			if( wk->btn_id >= BBAG_BGWF_RETURN ){
				pal = SYSBTN_PAL_ON;
			}else{
				pal = BUTTON_PAL_ON;
			}
			BGWINFRM_PaletteChange(
				wk->bgwfrm, wk->btn_id, 0, 0,
				ButtonAddData[wk->btn_id].sx, ButtonAddData[wk->btn_id].sy, pal );
			BGWINFRM_FrameOn( wk->bgwfrm, wk->btn_id );
			wk->btn_seq++;
			break;

		case 1:
			if( wk->btn_cnt == 4 ){
				if( wk->btn_id >= BBAG_BGWF_RETURN ){
					pal = SYSBTN_PAL_NORMAL;
				}else{
					pal = BUTTON_PAL_NORMAL;
				}
				BGWINFRM_PaletteChange(
					wk->bgwfrm, wk->btn_id, 0, 0,
					ButtonAddData[wk->btn_id].sx, ButtonAddData[wk->btn_id].sy, pal );
				BGWINFRM_FrameOn( wk->bgwfrm, wk->btn_id );
				wk->btn_cnt = 0;
				wk->btn_seq++;
			}else{
				wk->btn_cnt++;
			}
			break;

		case 2:
			if( wk->btn_cnt == 1 ){
				wk->btn_cnt = 0;
				wk->btn_seq = 0;
				wk->btn_flg = 0;
			}else{
				wk->btn_cnt++;
			}
			break;
		}
	}
}
