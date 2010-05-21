//============================================================================================
/**
 * @file		b_bag_obj.c
 * @brief		�퓬�p�o�b�O��� OBJ�֘A
 * @author	Hiroyuki Nakamura
 * @date		09.08.26
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "battle/battgra_wb.naix"

#include "b_app_tool.h"

#include "b_bag.h"
#include "b_bag_main.h"
#include "b_bag_obj.h"
#include "b_bag_item.h"
#include "b_bag_gra.naix"


//============================================================================================
//	�萔��`
//============================================================================================

#define	CURSOR_CLA_MAX		( 5 )		// �J�[�\����OBJ��
#define	FINGER_CLA_MAX		( 1 )		// �w�J�[�\����OBJ��

// �ߊl�f���̃J�[�\���\�����W
#define	P1_GETDEMO_CURSOR_X		( 192 )
#define	P1_GETDEMO_CURSOR_Y		( 24 )
#define	P2_GETDEMO_CURSOR_X		( 64 )
#define	P2_GETDEMO_CURSOR_Y		( 16 )
#define	P3_GETDEMO_CURSOR_X		( 104 )
#define	P3_GETDEMO_CURSOR_Y		( 152 )

#define	GETDEMO_DURSOR_PAL	( BBAG_PALRES_CURSOR + 1 )		// �ߊl�f���J�[�\���p���b�g�ʒu

// �R�X�g�o�[�A�j���ԍ�
#define	COST_ANM_ENE_NONE			( 15 )		// �G�l���M�[�Ȃ�
#define	COST_ANM_ENE_ONE			( 16 )		// �G�l���M�[�P
#define	COST_ANM_ENE_TWO			( 17 )		// �G�l���M�[�Q
#define	COST_ANM_ENE_RES_ONE	( 18 )		// �G�l���M�[�P�{�`���[�W�P
#define	COST_ANM_RES_ONE			( 19 )		// �`���[�W�P
#define	COST_ANM_RES_TWO			( 20 )		// �`���[�W�Q


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void BBAG_ClactResManInit( BBAG_WORK * wk );
static void BBAG_ClactItemLoad( BBAG_WORK * wk );
static void BBAG_ItemIconCharChg( BBAG_WORK * wk, u16 item, u32 chrResID, u32 palResID );
static void BBAG_ItemIconPlttChg( BBAG_WORK * wk, u16 item, u32 palResID );
static void BBAG_ClactAddAll( BBAG_WORK * wk );
static void BBAG_Page1ObjSet( BBAG_WORK * wk );
static void BBAG_Page2ObjSet( BBAG_WORK * wk );
static void BBAG_Page3ObjSet( BBAG_WORK * wk );
static void BBAG_ClactCursorAdd( BBAG_WORK * wk );
static void BBAG_CursorDel( BBAG_WORK * wk );
static void CostResLoad( BBAG_WORK * wk );
static void CursorResLoad( BBAG_WORK * wk );

static void InitGetDemoCursor( BBAG_WORK * wk );
static void ExitGetDemoCursor( BBAG_WORK * wk );
static void SetGetDemoCursor( BBAG_WORK * wk, int px, int py );
static void DelGetDemoCursor( BBAG_WORK * wk );

static void CostObjPut( BBAG_WORK * wk, u16 idx, u8 ene, u8 res_ene, const GFL_CLACTPOS * pos );


//============================================================================================
//	�O���[�o���ϐ�
//============================================================================================
// �y�[�W�P�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P1_ItemIconPos = { 36, 180 };

// �y�[�W�Q�̃A�C�e���A�C�R���̍��W
static const GFL_CLACTPOS P2_ItemIconPos[] =
{
	// �A�C�e���A�C�R��
	{ 36,  45 }, { 164,  45 },
	{ 36,  93 }, { 164,  93 },
	{ 36, 141 }, { 164, 141 },
	// �R�X�g
	{ 48+4,  41 }, { 176+4,  41 },
	{ 48+4,  89 }, { 176+4,  89 },
	{ 48+4, 137 }, { 176+4, 137 },
	// �G�l���M�[
	{ 88+12, 172 },
};

// �y�[�W�R�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P3_ItemIconPos[] =
{
	{  40, 44 },
	{ 168, 40 },
};

// �Z���A�N�^�[�̃f�[�^
static const u32 ClactDat[][3] =
{	// char, pal, cell
	{ BBAG_CHRRES_ITEM1, BBAG_PALRES_ITEM1, BBAG_CELRES_ITEM },
	{ BBAG_CHRRES_ITEM2, BBAG_PALRES_ITEM2, BBAG_CELRES_ITEM },
	{ BBAG_CHRRES_ITEM3, BBAG_PALRES_ITEM3, BBAG_CELRES_ITEM },
	{ BBAG_CHRRES_ITEM4, BBAG_PALRES_ITEM4, BBAG_CELRES_ITEM },
	{ BBAG_CHRRES_ITEM5, BBAG_PALRES_ITEM5, BBAG_CELRES_ITEM },
	{ BBAG_CHRRES_ITEM6, BBAG_PALRES_ITEM6, BBAG_CELRES_ITEM },
	{ BBAG_CHRRES_ITEM7, BBAG_PALRES_ITEM7, BBAG_CELRES_ITEM },

	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },

	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },

	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },

	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },

	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },

	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },

	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
	{ BBAG_CHRRES_COST, BBAG_PALRES_COST, BBAG_CELRES_COST },
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		�퓬�p�o�b�OOBJ������
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_ObjInit( BBAG_WORK * wk )
{
	BBAG_ClactResManInit( wk );
	BBAG_ClactItemLoad( wk );
	CostResLoad( wk );
	CursorResLoad( wk );

	BBAG_ClactAddAll( wk );
	BBAG_ClactCursorAdd( wk );

	InitGetDemoCursor( wk );

	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���\�[�X�}�l�[�W���[������
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_ClactResManInit( BBAG_WORK * wk )
{
	u32	i;

	for( i=0; i<BBAG_CHRRES_MAX; i++ ){
		wk->chrRes[i] = 0xffffffff;
	}
	for( i=0; i<BBAG_PALRES_MAX; i++ ){
		wk->palRes[i] = 0xffffffff;
	}
	for( i=0; i<BBAG_CELRES_MAX; i++ ){
		wk->celRes[i] = 0xffffffff;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���A�C�R���̃O���t�B�b�N���[�h
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_ClactItemLoad( BBAG_WORK * wk )
{
  ARCHANDLE * ah;
	u8	cgx_type, pal_type;
	u16	dmyItem;
	u16	i;

	if( wk->dat->mode == BBAG_MODE_SHOOTER ){
		dmyItem  = SHOOTER_ITEM_ShooterIndexToItemIndex( 0 );
		cgx_type = ITEM_GET_SHOOTER_ICON_CGX;
		pal_type = ITEM_GET_SHOOTER_ICON_PAL;
	}else{
		dmyItem  = 1;
		cgx_type = ITEM_GET_ICON_CGX;
		pal_type = ITEM_GET_ICON_PAL;
	}

	ah = GFL_ARC_OpenDataHandle( ITEM_GetIconArcID(), GFL_HEAP_LOWID(wk->dat->heap) );

	for( i=0; i<7; i++ ){
	  wk->chrRes[BBAG_CHRRES_ITEM1+i] = GFL_CLGRP_CGR_Register(
																				ah, ITEM_GetIndex(dmyItem,cgx_type),
																				FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
	  wk->palRes[BBAG_PALRES_ITEM1+i] = GFL_CLGRP_PLTT_Register(
																				ah, ITEM_GetIndex(dmyItem,pal_type),
																				CLSYS_DRAW_SUB, i*32, wk->dat->heap );
	}
  wk->celRes[BBAG_CELRES_ITEM] = GFL_CLGRP_CELLANIM_Register(
																	ah, ITEM_GetIconCell(),  ITEM_GetIconCellAnm(), wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���A�C�R���L�����؂�ւ�
 *
 * @param		wk			���[�N
 * @param		item		�A�C�e���ԍ�
 * @param		res_id	�L�������\�[�XID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_ItemIconCharChg( BBAG_WORK * wk, u16 item, u32 chrResID, u32 palResID )
{
	NNSG2dCharacterData * dat;
	void * buf;
	ARCHANDLE * ah;
	u8	cgx_type, pal_type;

	if( wk->dat->mode == BBAG_MODE_SHOOTER ){
		cgx_type = ITEM_GET_SHOOTER_ICON_CGX;
		pal_type = ITEM_GET_SHOOTER_ICON_PAL;
	}else{
		cgx_type = ITEM_GET_ICON_CGX;
		pal_type = ITEM_GET_ICON_PAL;
	}

	ah = GFL_ARC_OpenDataHandle( ITEM_GetIconArcID(), GFL_HEAP_LOWID(wk->dat->heap) );

	// �L����
	buf = GFL_ARCHDL_UTIL_LoadOBJCharacter(
					ah, ITEM_GetIndex(item,cgx_type), FALSE, &dat, wk->dat->heap );
	GFL_CLGRP_CGR_Replace( wk->chrRes[chrResID], dat );
	GFL_HEAP_FreeMemory( buf );

	// �p���b�g
	PaletteWorkSet_ArcHandle(
					wk->pfd, ah, ITEM_GetIndex(item,pal_type),
					wk->dat->heap, FADE_SUB_OBJ, 0x20, palResID * 16 );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���A�C�R���p���b�g�؂�ւ�
 *
 * @param		wk			���[�N
 * @param		item		�A�C�e���ԍ�
 * @param		pos			�ʒu
 * @param		res_id	�p���b�g���\�[�XID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_ItemIconPlttChg( BBAG_WORK * wk, u16 item, u32 palResID )
{
	if( wk->dat->mode == BBAG_MODE_SHOOTER ){
		GFL_ARC_UTIL_TransVramPalette(
			ITEM_GetIconArcID(),
			ITEM_GetIndex(item,ITEM_GET_SHOOTER_ICON_PAL),
			PALTYPE_SUB_OBJ, palResID * 0x20, 0x20, wk->dat->heap );
	}else{
		GFL_ARC_UTIL_TransVramPalette(
			ITEM_GetIconArcID(),
			ITEM_GetIndex(item,ITEM_GET_ICON_PAL),
			PALTYPE_SUB_OBJ, palResID * 0x20, 0x20, wk->dat->heap );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���\�[�X���[�h�i�R�X�g�j
 *
 * @param		wk			���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CostResLoad( BBAG_WORK * wk )
{
  ARCHANDLE * ah;
	u32	i;
	
	ah = GFL_ARC_OpenDataHandle( ARCID_B_BAG_GRA, GFL_HEAP_LOWID(wk->dat->heap) );

  wk->chrRes[BBAG_CHRRES_COST] = GFL_CLGRP_CGR_Register(
																		ah, NARC_b_bag_gra_test_cost_lz_NCGR,
																		TRUE, CLSYS_DRAW_SUB, wk->dat->heap );
  wk->palRes[BBAG_PALRES_COST] = GFL_CLGRP_PLTT_Register(
																		ah, NARC_b_bag_gra_test_cost_NCLR,
																		CLSYS_DRAW_SUB, BBAG_PALRES_COST*32, wk->dat->heap );
  wk->celRes[BBAG_CELRES_COST] = GFL_CLGRP_CELLANIM_Register(
																	ah, NARC_b_bag_gra_test_cost_NCER,
																	NARC_b_bag_gra_test_cost_NANR, wk->dat->heap );
	// �p���b�g
	PaletteWorkSet_ArcHandle(
					wk->pfd, ah, NARC_b_bag_gra_test_cost_NCLR,
					wk->dat->heap, FADE_SUB_OBJ, 0x20, BBAG_PALRES_COST * 16 );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���\�[�X���[�h�i�J�[�\���j
 *
 * @param		wk			���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CursorResLoad( BBAG_WORK * wk )
{
  ARCHANDLE * ah;

	ah = GFL_ARC_OpenDataHandle( ARCID_BATTGRA, GFL_HEAP_LOWID(wk->dat->heap) );

	// �L����
	wk->chrRes[BBAG_CHRRES_CURSOR] = GFL_CLGRP_CGR_Register(
																		ah, NARC_battgra_wb_battle_w_cursor_NCGR,
																		FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
	// �p���b�g
  wk->palRes[BBAG_PALRES_CURSOR] = GFL_CLGRP_PLTT_RegisterEx(
																		ah, NARC_battgra_wb_battle_w_obj_NCLR,
																		CLSYS_DRAW_SUB, BBAG_PALRES_CURSOR*0x20, 4, 1, wk->dat->heap );
	// �Z���E�A�j��
  wk->celRes[BBAG_CELRES_CURSOR] = GFL_CLGRP_CELLANIM_Register(
																		ah,
																		NARC_battgra_wb_battle_w_cursor_NCER,
																		NARC_battgra_wb_battle_w_cursor_NANR,
																		wk->dat->heap );

	// �p���b�g�t�F�[�h�ɐݒ�
	{
		NNSG2dPaletteData * dat;
		void * buf;

		buf = GFL_ARCHDL_UTIL_LoadPalette( ah, NARC_battgra_wb_battle_w_obj_NCLR, &dat, GFL_HEAP_LOWID(wk->dat->heap) );
		PaletteWorkSet(
			wk->pfd, &(((u16*)(dat->pRawData))[16*4]), FADE_SUB_OBJ, BBAG_PALRES_CURSOR*16, 0x20 );
	
		GFL_HEAP_FreeMemory( buf );
	}

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�ǉ��i�ʁj
 *
 * @param		wk		���[�N
 * @param		id		�ǉ�����Z���A�N�^�[��ID
 *
 * @return	�Z���A�N�^�[�f�[�^
 */
//--------------------------------------------------------------------------------------------
static GFL_CLWK * BBAG_ClactAdd( BBAG_WORK * wk, const u32 * res )
{
	GFL_CLWK * clwk;
	GFL_CLWK_DATA	dat;

	dat.pos_x = 0;
	dat.pos_y = 0;
	dat.anmseq = 0;
	dat.softpri = 1;
	dat.bgpri = 1;

	clwk = GFL_CLACT_WK_Create(
						wk->clunit,
						wk->chrRes[res[0]],
						wk->palRes[res[1]],
						wk->celRes[res[2]],
						&dat, CLSYS_DRAW_SUB, wk->dat->heap );

	return clwk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�ǉ��i�S�āj
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_ClactAddAll( BBAG_WORK * wk )
{
	u32	i;

	wk->clunit = GFL_CLACT_UNIT_Create( BBAG_CA_MAX+BAPPTOOL_CURSOR_MAX, 0, wk->dat->heap );

	for( i=0; i<BBAG_CA_MAX; i++ ){
		wk->clwk[i] = BBAG_ClactAdd( wk, ClactDat[i] );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�퓬�p�o�b�OOBJ�폜
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_ObjFree( BBAG_WORK * wk )
{
	u32	i;

	ExitGetDemoCursor( wk );

	for( i=0; i<BBAG_CA_MAX; i++ ){
		GFL_CLACT_WK_Remove( wk->clwk[i] );
	}
	BBAG_CursorDel( wk );

	for( i=0; i<BBAG_CHRRES_MAX; i++ ){
		if( wk->chrRes[i] != 0xffffffff ){
			GFL_CLGRP_CGR_Release( wk->chrRes[i] );
		}
	}
	for( i=0; i<BBAG_PALRES_MAX; i++ ){
		if( wk->palRes[i] != 0xffffffff ){
	    GFL_CLGRP_PLTT_Release( wk->palRes[i] );
		}
	}
	for( i=0; i<BBAG_CELRES_MAX; i++ ){
		if( wk->celRes[i] != 0xffffffff ){
	    GFL_CLGRP_CELLANIM_Release( wk->celRes[i] );
		}
	}

	GFL_CLACT_UNIT_Delete( wk->clunit );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[��\�����č��W�ύX
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_ClactOn( GFL_CLWK * clwk, const GFL_CLACTPOS * pos )
{
	GFL_CLACT_WK_SetDrawEnable( clwk, TRUE );
	GFL_CLACT_WK_SetPos( clwk, pos, CLSYS_DRAW_SUB );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W���Ƃ�OBJ���Z�b�g
 *
 * @param		wk		���[�N
 * @param		page	�y�[�W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_PageObjSet( BBAG_WORK * wk, u32 page )
{
	u32	i;

	for( i=0; i<BBAG_CA_MAX; i++ ){
		GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], FALSE );
	}

	DelGetDemoCursor( wk );

	switch( page ){
	case BBAG_PAGE_POCKET:		// �|�P�b�g�I���y�[�W
		BBAG_Page1ObjSet( wk );
		break;
	case BBAG_PAGE_MAIN:		// �A�C�e���I���y�[�W
		BBAG_Page2ObjSet( wk );
		break;
	case BBAG_PAGE_ITEM:		// �A�C�e���g�p�y�[�W
		BBAG_Page3ObjSet( wk );
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�b�g�I���y�[�W��OBJ���Z�b�g
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_Page1ObjSet( BBAG_WORK * wk )
{
	u16	item;

	if( wk->used_item != ITEM_DUMMY_DATA ){
		BBAG_ItemIconCharChg( wk, wk->used_item, BBAG_CHRRES_ITEM7, BBAG_PALRES_ITEM7 );
		BBAG_ClactOn( wk->clwk[BBAG_CA_ITEM7], &P1_ItemIconPos );
	}

	if( wk->dat->mode == BBAG_MODE_GETDEMO ){
		SetGetDemoCursor( wk, P1_GETDEMO_CURSOR_X, P1_GETDEMO_CURSOR_Y );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���I���y�[�W��OBJ���Z�b�g
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_Page2ObjSet( BBAG_WORK * wk )
{
	u32	i;
	u16	item;

	for( i=0; i<6; i++ ){
		item = BattleBag_PosItemCheck( wk, i );
		if( item == 0 ){ continue; }
		BBAG_ItemIconCharChg( wk, item, BBAG_CHRRES_ITEM1+i, BBAG_PALRES_ITEM1+i );
		BBAG_ItemIconPlttChg( wk, item, BBAG_PALRES_ITEM1+i );
		BBAG_ClactOn( wk->clwk[BBAG_CA_ITEM1+i], &P2_ItemIconPos[i] );

		if( wk->dat->mode == BBAG_MODE_SHOOTER ){
			CostObjPut( wk, BBAG_CA_COST1_NUM+i*8, BBAGITEM_GetCost(item), 0, &P2_ItemIconPos[6+i] );
		}
	}

	if( wk->dat->mode == BBAG_MODE_SHOOTER ){
		CostObjPut( wk, BBAG_CA_ENERGIE_NUM, wk->dat->energy, wk->dat->reserved_energy, &P2_ItemIconPos[6+i] );
	}

	if( wk->dat->mode == BBAG_MODE_GETDEMO ){
		SetGetDemoCursor( wk, P2_GETDEMO_CURSOR_X, P2_GETDEMO_CURSOR_Y );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���g�p�y�[�W��OBJ���Z�b�g
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_Page3ObjSet( BBAG_WORK * wk )
{
	u16	item;

	item = BattleBag_PosItemCheck( wk, wk->dat->item_pos[wk->poke_id] );
	BBAG_ItemIconCharChg( wk, item, BBAG_CHRRES_ITEM7, BBAG_PALRES_ITEM7 );
	BBAG_ClactOn( wk->clwk[BBAG_CA_ITEM7], &P3_ItemIconPos[0] );

	if( wk->dat->mode == BBAG_MODE_SHOOTER ){
			CostObjPut( wk, BBAG_CA_COST1_NUM, BBAGITEM_GetCost(item), 0, &P3_ItemIconPos[1] );
	}

	if( wk->dat->mode == BBAG_MODE_GETDEMO ){
		SetGetDemoCursor( wk, P3_GETDEMO_CURSOR_X, P3_GETDEMO_CURSOR_Y );
	}
}


//============================================================================================
//	�J�[�\��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�I���J�[�\���ǉ�
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_ClactCursorAdd( BBAG_WORK * wk )
{
	BAPPTOOL_AddCursor(
		wk->cpwk, wk->clunit,
		wk->chrRes[BBAG_CHRRES_CURSOR],
		wk->palRes[BBAG_PALRES_CURSOR],
		wk->celRes[BBAG_CELRES_CURSOR] );

	BAPPTOOL_VanishCursor( wk->cpwk, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�I���J�[�\���폜
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_CursorDel( BBAG_WORK * wk )
{
	BAPPTOOL_DelCursor( wk->cpwk );
}


//============================================================================================
//	�ߊl�f���J�[�\��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ߊl�f���J�[�\��������
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitGetDemoCursor( BBAG_WORK * wk )
{
	wk->getdemoCursor = BTLV_FINGER_CURSOR_Init( wk->pfd, GETDEMO_DURSOR_PAL, wk->dat->heap );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ߊl�f���J�[�\���폜
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitGetDemoCursor( BBAG_WORK * wk )
{
	BTLV_FINGER_CURSOR_Exit( wk->getdemoCursor );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ߊl�f���J�[�\���Z�b�g
 *
 * @param		wk		���[�N
 * @param		px		�w���W
 * @param		py		�x���W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetGetDemoCursor( BBAG_WORK * wk, int px, int py )
{
	BTLV_FINGER_CURSOR_Create( wk->getdemoCursor, px, py, 2, 6, 20 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ߊl�f���J�[�\���폜
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void DelGetDemoCursor( BBAG_WORK * wk )
{
	BTLV_FINGER_CURSOR_Delete( wk->getdemoCursor );
}


//============================================================================================
//	�R�X�g
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�X�g�z�u
 *
 * @param		wk				���[�N
 * @param		idx				OBJ�C���f�b�N�X
 * @param		ene				���݂̃G�l���M�[
 * @param		res_ene		����Ȃ��G�l���M�[
 * @param		pos				�\�����W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CostObjPut( BBAG_WORK * wk, u16 idx, u8 ene, u8 res_ene, const GFL_CLACTPOS * pos )
{
	GFL_CLACTPOS	mvp;
	u16	i;
	s8	te, re;

	// ����
	BBAG_ClactOn( wk->clwk[idx], pos );
	GFL_CLACT_WK_SetAnmSeq( wk->clwk[idx], ene );

	// �Q�[�W
	idx++;
	te = ene;
	re = res_ene;
	mvp = *pos;
	mvp.x += 12;
	for( i=0; i<7; i++ ){
		if( te <= 0 ){
			if( re <= 0 ){
				GFL_CLACT_WK_SetAnmSeq( wk->clwk[idx], COST_ANM_ENE_NONE );
			}else if( re == 1 ){
				GFL_CLACT_WK_SetAnmSeq( wk->clwk[idx], COST_ANM_RES_ONE );
				re -= 1;
			}else{
				GFL_CLACT_WK_SetAnmSeq( wk->clwk[idx], COST_ANM_RES_TWO );
				re -= 2;
			}
		}else if( te == 1 ){
			if( re == 0 ){
				GFL_CLACT_WK_SetAnmSeq( wk->clwk[idx], COST_ANM_ENE_ONE );
			}else if( re > 0 ){
				GFL_CLACT_WK_SetAnmSeq( wk->clwk[idx], COST_ANM_ENE_RES_ONE );
				re -= 1;
			}
			te -= 1;
		}else{
			GFL_CLACT_WK_SetAnmSeq( wk->clwk[idx], COST_ANM_ENE_TWO );
			te -= 2;
		}
		BBAG_ClactOn( wk->clwk[idx], &mvp );
		mvp.x += 8;
		idx++;
	}
}
