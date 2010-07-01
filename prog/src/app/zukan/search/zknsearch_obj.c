//============================================================================================
/**
 * @file		zknsearch_obj.c
 * @brief		�}�ӌ������ �n�a�i�֘A
 * @author	Hiroyuki Nakamura
 * @date		10.02.09
 *
 *	���W���[�����FZKNSEARCHOBJ
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/main.h"
#include "app/app_menu_common.h"
#include "ui/touchbar.h"

#include "../zukan_common.h"
#include "zknsearch_main.h"
#include "zknsearch_list.h"
#include "zknsearch_obj.h"
#include "zukan_gra.naix"


//============================================================================================
//	�萔��`
//============================================================================================
#define	RES_NONE	( 0xffffffff )		// ���\�[�X���ǂݍ��܂�Ă��Ȃ�

// �p���b�g��`�i���C����ʁj
#define	PALNUM_ZKNOBJ			( 0 )															// �p���b�g�ԍ��F�}�ӗp�n�a�i
#define	PALSIZ_ZKNOBJ			(	3 )															// �p���b�g�T�C�Y�F�}�ӗp�n�a�i
#define	PALNUM_LOADING		( PALNUM_ZKNOBJ+PALSIZ_ZKNOBJ )		// �p���b�g�ԍ��F�ǂݍ��ݒ��n�a�i
#define	PALSIZ_LOADING		(	1 )															// �p���b�g�T�C�Y�F�ǂݍ��ݒ��n�a�i
#define	PALNUM_FORM				( PALNUM_LOADING+PALSIZ_LOADING )	// �p���b�g�ԍ��F�t�H�����n�a�i
#define	PALSIZ_FORM				(	1 )															// �p���b�g�T�C�Y�F�t�H�����n�a�i
#define	PALNUM_TOUCH_BAR	( PALNUM_FORM+PALSIZ_FORM )				// �p���b�g�ԍ��F�^�b�`�o�[
#define	PALSIZ_TOUCH_BAR	( 4 )															// �p���b�g�T�C�Y�F�^�b�`�o�[
// �p���b�g��`�i�T�u��ʁj
#define	PALNUM_ZKNOBJ_S		( 0 )																// �p���b�g�ԍ��F�}�ӗp�n�a�i
#define	PALSIZ_ZKNOBJ_S		(	3 )																// �p���b�g�T�C�Y�F�}�ӗp�n�a�i
#define	PALNUM_FORM_S			( PALNUM_ZKNOBJ_S+PALSIZ_ZKNOBJ_S )	// �p���b�g�ԍ��F�t�H�����n�a�i
#define	PLASIZ_FORM_S			( PALSIZ_FORM )											// �p���b�g�T�C�Y�F�t�H�����n�a�i

// �`�F�b�N�}�[�N�\�����W
#define	MARK_PX				( 140 )
#define	MARK_PY				( 12 )
#define	MARK_SY				( 24 )
// �`�F�b�N�}�[�N�\�����W�i�t�H�����p�j
#define	MARK_PX_FORM	( 140 )
#define	MARK_PY_FORM	( 20 )
#define	MARK_SY_FORM	( 40 )

// ���X�g��̃t�H�����\�����W
#define	FORM_PX				( 204 )
#define	FORM_PY				( 20 )

// �X�N���[���o�[�\�����W
#define	SCROLL_BAR_PX	( 252 )
#define	SCROLL_BAR_UY	( 12 )
#define	SCROLL_BAR_DY	( 156 )

// ���C���y�[�W�̃t�H�����\�����W
#define	MAINPAGE_FORM_PX	( 192 )
#define	MAINPAGE_FORM_PY	( 116 )

// �t�H�����y�[�W�̃t�H�����\�����W
#define	FORMPAGE_FORM_PX	( 64 )
#define	FORMPAGE_FORM_PY	( 104 )

// �ǂݍ��ݒ��o�[�\�����W
#define	LOADING_BAR_PX		( 8 )
#define	LOADING_BAR_PY		( 168 )
#define	LOADING_BAR_MVX		( 2 )


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void InitClact(void);
static void InitResource( ZKNSEARCHMAIN_WORK * wk );
static void ExitResource( ZKNSEARCHMAIN_WORK * wk );
static void AddClact( ZKNSEARCHMAIN_WORK * wk );
static void DelClactAll( ZKNSEARCHMAIN_WORK * wk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// �Z���A�N�^�[�f�[�^
static const ZKNCOMM_CLWK_DATA ClactParamTbl[] =
{
	{	// �X�N���[���o�[
		{ SCROLL_BAR_PX, SCROLL_BAR_UY, ZKNSEARCHOBJ_ANM_BAR, 0, 1 },
		ZKNSEARCHOBJ_CHRRES_SCROLL_BAR, ZKNSEARCHOBJ_PALRES_SCROLL_BAR, ZKNSEARCHOBJ_CELRES_SCROLL_BAR,
		0, CLSYS_DRAW_MAIN
	},
	{	// ���[��
		{ SCROLL_BAR_PX, 84, ZKNSEARCHOBJ_ANM_RAIL, 10, 1 },
		ZKNSEARCHOBJ_CHRRES_SCROLL_BAR, ZKNSEARCHOBJ_PALRES_SCROLL_BAR, ZKNSEARCHOBJ_CELRES_SCROLL_BAR,
		0, CLSYS_DRAW_MAIN
	},
	{	// �߂�{�^��
		{ 232, TOUCHBAR_ICON_Y, APP_COMMON_BARICON_RETURN, 0, 0 },
		ZKNSEARCHOBJ_CHRRES_TOUCH_BAR, ZKNSEARCHOBJ_PALRES_TOUCH_BAR, ZKNSEARCHOBJ_CELRES_TOUCH_BAR,
		0, CLSYS_DRAW_MAIN
	},
	{	// �w�{�^��
		{ 208, TOUCHBAR_ICON_Y, APP_COMMON_BARICON_EXIT, 0, 0 },
		ZKNSEARCHOBJ_CHRRES_TOUCH_BAR, ZKNSEARCHOBJ_PALRES_TOUCH_BAR, ZKNSEARCHOBJ_CELRES_TOUCH_BAR,
		0, CLSYS_DRAW_MAIN
	},
	{	// �x�{�^��
		{ 188, TOUCHBAR_ICON_Y_CHECK, APP_COMMON_BARICON_CHECK_OFF, 0, 0 },
		ZKNSEARCHOBJ_CHRRES_TOUCH_BAR, ZKNSEARCHOBJ_PALRES_TOUCH_BAR, ZKNSEARCHOBJ_CELRES_TOUCH_BAR,
		0, CLSYS_DRAW_MAIN
	},
	{	// �E�{�^��
		{ 208, TOUCHBAR_ICON_Y, APP_COMMON_BARICON_CURSOR_RIGHT, 0, 0 },
		ZKNSEARCHOBJ_CHRRES_TOUCH_BAR, ZKNSEARCHOBJ_PALRES_TOUCH_BAR, ZKNSEARCHOBJ_CELRES_TOUCH_BAR,
		0, CLSYS_DRAW_MAIN
	},
	{	// ���{�^��
		{ 188, TOUCHBAR_ICON_Y, APP_COMMON_BARICON_CURSOR_LEFT, 0, 0 },
		ZKNSEARCHOBJ_CHRRES_TOUCH_BAR, ZKNSEARCHOBJ_PALRES_TOUCH_BAR, ZKNSEARCHOBJ_CELRES_TOUCH_BAR,
		0, CLSYS_DRAW_MAIN
	},
	{	// �t�H����
		{ 0, 0, 0, 0, 0 },
		ZKNSEARCHOBJ_CHRRES_FORM_M, ZKNSEARCHOBJ_PALRES_FORM_M, ZKNSEARCHOBJ_CELRES_FORM,
		0, CLSYS_DRAW_MAIN
	},
	{	// �ǂݍ��ݒ�
		{ LOADING_BAR_PX, LOADING_BAR_PY, 0, 0, 1 },
		ZKNSEARCHOBJ_CHRRES_LOADING_BAR, ZKNSEARCHOBJ_PALRES_LOADING_BAR, ZKNSEARCHOBJ_CELRES_LOADING_BAR,
		0, CLSYS_DRAW_MAIN
	},
	{	// �}�[�N�P
		{ 0, 0, 2, 0, 1 },
		ZKNSEARCHOBJ_CHRRES_SCROLL_BAR, ZKNSEARCHOBJ_PALRES_SCROLL_BAR, ZKNSEARCHOBJ_CELRES_SCROLL_BAR,
		0, CLSYS_DRAW_MAIN
	},
	{	// �}�[�N�Q
		{ 0, 0, 2, 0, 1 },
		ZKNSEARCHOBJ_CHRRES_SCROLL_BAR, ZKNSEARCHOBJ_PALRES_SCROLL_BAR, ZKNSEARCHOBJ_CELRES_SCROLL_BAR,
		0, CLSYS_DRAW_MAIN
	},
	{	// �}�[�N�P
		{ 0, 0, 2, 0, 2 },
		ZKNSEARCHOBJ_CHRRES_SCROLL_BAR_S, ZKNSEARCHOBJ_PALRES_SCROLL_BAR_S, ZKNSEARCHOBJ_CELRES_SCROLL_BAR,
		0, CLSYS_DRAW_SUB
	},
	{	// �}�[�N�Q
		{ 0, 0, 2, 0, 2 },
		ZKNSEARCHOBJ_CHRRES_SCROLL_BAR_S, ZKNSEARCHOBJ_PALRES_SCROLL_BAR_S, ZKNSEARCHOBJ_CELRES_SCROLL_BAR,
		0, CLSYS_DRAW_SUB
	},
	{	// �}�[�N�P ( OBJ WINDOW )
		{ 0, 0, 2, 0, 2 },
		ZKNSEARCHOBJ_CHRRES_SCROLL_BAR_S, ZKNSEARCHOBJ_PALRES_SCROLL_BAR_S, ZKNSEARCHOBJ_CELRES_SCROLL_BAR,
		0, CLSYS_DRAW_SUB
	},
	{	// �}�[�N�Q ( OBJ WINDOW )
		{ 0, 0, 2, 0, 2 },
		ZKNSEARCHOBJ_CHRRES_SCROLL_BAR_S, ZKNSEARCHOBJ_PALRES_SCROLL_BAR_S, ZKNSEARCHOBJ_CELRES_SCROLL_BAR,
		0, CLSYS_DRAW_SUB
	},
};

// �t�H�����Z���A�N�^�[�f�[�^�i���C����ʁj
static const ZKNCOMM_CLWK_DATA FormClactParamMain = {
	{ 0, 0, 0, 0, 1 },
	ZKNSEARCHOBJ_CHRRES_FORM_M, ZKNSEARCHOBJ_PALRES_FORM_M, ZKNSEARCHOBJ_CELRES_FORM,
	0, CLSYS_DRAW_MAIN
};
// �t�H�����Z���A�N�^�[�f�[�^�i�T�u��ʁj
static const ZKNCOMM_CLWK_DATA FormClactParamSub = {
	{ 0, 0, 0, 0, 2 },
	ZKNSEARCHOBJ_CHRRES_FORM_S, ZKNSEARCHOBJ_PALRES_FORM_S, ZKNSEARCHOBJ_CELRES_FORM,
	0, CLSYS_DRAW_SUB
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[������
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_Init( ZKNSEARCHMAIN_WORK * wk )
{
	InitClact();
	InitResource( wk );
	AddClact( wk );

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// MAIN DISP OBJ ON
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// SUB DISP OBJ ON
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�폜
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_Exit( ZKNSEARCHMAIN_WORK * wk )
{
	DelClactAll( wk );
	ExitResource( wk );
	GFL_CLACT_SYS_Delete();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j�����C��
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_AnmMain( ZKNSEARCHMAIN_WORK * wk )
{
	u32	i;

	for( i=0; i<ZKNSEARCHOBJ_IDX_MAX; i++ ){
		if( wk->clwk[i] == NULL ){ continue; }
		if( GFL_CLACT_WK_GetAutoAnmFlag( wk->clwk[i] ) == TRUE ){ continue; }
		GFL_CLACT_WK_AddAnmFrame( wk->clwk[i], FX32_ONE );
	}
	GFL_CLACT_SYS_Main();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j���ύX
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 * @param		anm		�A�j���ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_SetAnm( ZKNSEARCHMAIN_WORK * wk, u32 id, u32 anm )
{
	GFL_CLACT_WK_SetAnmFrame( wk->clwk[id], 0 );
	GFL_CLACT_WK_SetAnmSeq( wk->clwk[id], anm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�I�[�g�A�j���ݒ�
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 * @param		anm		�A�j���ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_SetAutoAnm( ZKNSEARCHMAIN_WORK * wk, u32 id, u32 anm )
{
	ZKNSEARCHOBJ_SetAnm( wk, id, anm );
	GFL_CLACT_WK_SetAutoAnmFlag( wk->clwk[id], TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j���擾
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 *
 * @return	�A�j���ԍ�
 */
//--------------------------------------------------------------------------------------------
u32 ZKNSEARCHOBJ_GetAnm( ZKNSEARCHMAIN_WORK * wk, u32 id )
{
	return GFL_CLACT_WK_GetAnmSeq( wk->clwk[id] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j����Ԏ擾
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 *
 * @retval	"TRUE = �A�j����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZKNSEARCHOBJ_CheckAnm( ZKNSEARCHMAIN_WORK * wk, u32 id )
{
	return GFL_CLACT_WK_CheckAnmActive( wk->clwk[id] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�\���؂�ւ�
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 * @param		flg		�\���t���O
 *
 * @return	none
 *
 * @li	flg = TRUE : �\��
 * @li	flg = FALSE : ��\��
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_SetVanish( ZKNSEARCHMAIN_WORK * wk, u32 id, BOOL flg )
{
	if( wk->clwk[id] != NULL ){
		GFL_CLACT_WK_SetDrawEnable( wk->clwk[id], flg );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�\���`�F�b�N
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 *
 * @retval	"TRUE = �\��"
 * @retval	"FALSE = ��\��"
 */
//--------------------------------------------------------------------------------------------
BOOL ZKNSEARCHOBJ_CheckVanish( ZKNSEARCHMAIN_WORK * wk, u32 id )
{
	return GFL_CLACT_WK_GetDrawEnable( wk->clwk[id] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�������ݒ�
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 * @param		flg		ON/OFF�t���O
 *
 * @return	none
 *
 * @li	flg : TRUE = ON, FALSE = OFF
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_SetBlendMode( ZKNSEARCHMAIN_WORK * wk, u32 id, BOOL flg )
{
	if( flg == TRUE ){
		GFL_CLACT_WK_SetObjMode( wk->clwk[id], GX_OAM_MODE_XLU );
	}else{
		GFL_CLACT_WK_SetObjMode( wk->clwk[id], GX_OAM_MODE_NORMAL );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[���W�ݒ�
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		id			OBJ ID
 * @param		x				�w���W
 * @param		y				�x���W
 * @param		setsf		�\�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_SetPos( ZKNSEARCHMAIN_WORK * wk, u32 id, s16 x, s16 y, u16 setsf )
{
	GFL_CLACTPOS	pos;

	pos.x = x;
	pos.y = y;
	GFL_CLACT_WK_SetPos( wk->clwk[id], &pos, setsf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[���W�擾
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		id			OBJ ID
 * @param		x				�w���W
 * @param		y				�x���W
 * @param		setsf		�\�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_GetPos( ZKNSEARCHMAIN_WORK * wk, u32 id, s16 * x, s16 * y, u16 setsf )
{
	GFL_CLACTPOS	pos;

	GFL_CLACT_WK_GetPos( wk->clwk[id], &pos, setsf );
	*x = pos.x;
	*y = pos.y;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[BG�v���C�I���e�B�ύX
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		id			OBJ ID
 * @param		pri			�v���C�I���e�B
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_BgPriChange( ZKNSEARCHMAIN_WORK * wk, u32 id, int pri )
{
	GFL_CLACT_WK_SetBgPri( wk->clwk[id], pri );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[������
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitClact(void)
{
	const GFL_CLSYS_INIT init = {
		0, 0,									// ���C���@�T�[�t�F�[�X�̍�����W
		0, 512,								// �T�u�@�T�[�t�F�[�X�̍�����W
		4,										// ���C�����OAM�Ǘ��J�n�ʒu	4�̔{��
		124,									// ���C�����OAM�Ǘ���				4�̔{��
		4,										// �T�u���OAM�Ǘ��J�n�ʒu		4�̔{��
		124,									// �T�u���OAM�Ǘ���					4�̔{��
		0,										// �Z��Vram�]���Ǘ���

		ZKNSEARCHOBJ_CHRRES_MAX,	// �o�^�ł���L�����f�[�^��
		ZKNSEARCHOBJ_PALRES_MAX,	// �o�^�ł���p���b�g�f�[�^��
		ZKNSEARCHOBJ_CELRES_MAX,	// �o�^�ł���Z���A�j���p�^�[����
		0,											// �o�^�ł���}���`�Z���A�j���p�^�[�����i�����󖢑Ή��j

	  16,										// ���C�� CGR�@VRAM�Ǘ��̈�@�J�n�I�t�Z�b�g�i�L�����N�^�P�ʁj
	  16										// �T�u CGR�@VRAM�Ǘ��̈�@�J�n�I�t�Z�b�g�i�L�����N�^�P�ʁj
	};
	GFL_CLACT_SYS_Create( &init, ZKNSEARCHMAIN_GetVramBankData(), HEAPID_ZUKAN_SEARCH );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���\�[�X������
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitResource( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah;
	u32 * chr;
	u32 * pal;
	u32 * cel;
	u32	i;

	// ������
	for( i=0; i<ZKNSEARCHOBJ_CHRRES_MAX; i++ ){
		wk->chrRes[i] = RES_NONE;
	}
	for( i=0; i<ZKNSEARCHOBJ_PALRES_MAX; i++ ){
		wk->palRes[i] = RES_NONE;
	}
	for( i=0; i<ZKNSEARCHOBJ_CELRES_MAX; i++ ){
		wk->celRes[i] = RES_NONE;
	}

	ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );
	// �X�N���[���o�[
	// ���C�����
	chr = &wk->chrRes[ ZKNSEARCHOBJ_CHRRES_SCROLL_BAR ];
	pal = &wk->palRes[ ZKNSEARCHOBJ_PALRES_SCROLL_BAR ];
	cel = &wk->celRes[ ZKNSEARCHOBJ_CELRES_SCROLL_BAR ];
	*chr = GFL_CLGRP_CGR_Register(
					ah, NARC_zukan_gra_search_search_objd_NCGR,
					FALSE, CLSYS_DRAW_MAIN, HEAPID_ZUKAN_SEARCH );
  *pal = GFL_CLGRP_PLTT_Register(
					ah, NARC_zukan_gra_search_search_objd_NCLR,
					CLSYS_DRAW_MAIN, PALNUM_ZKNOBJ*0x20, HEAPID_ZUKAN_SEARCH );
	*cel = GFL_CLGRP_CELLANIM_Register(
					ah,
					NARC_zukan_gra_search_search_objd_NCER,
					NARC_zukan_gra_search_search_objd_NANR,
					HEAPID_ZUKAN_SEARCH );
	// �T�u���
	chr = &wk->chrRes[ ZKNSEARCHOBJ_CHRRES_SCROLL_BAR_S ];
	pal = &wk->palRes[ ZKNSEARCHOBJ_PALRES_SCROLL_BAR_S ];
	*chr = GFL_CLGRP_CGR_Register(
					ah, NARC_zukan_gra_search_search_objd_NCGR,
					FALSE, CLSYS_DRAW_SUB, HEAPID_ZUKAN_SEARCH );
  *pal = GFL_CLGRP_PLTT_Register(
					ah, NARC_zukan_gra_search_search_objd_NCLR,
					CLSYS_DRAW_SUB, PALNUM_ZKNOBJ_S*0x20, HEAPID_ZUKAN_SEARCH );
	// ���[�f�B���O�o�[
	chr = &wk->chrRes[ ZKNSEARCHOBJ_CHRRES_LOADING_BAR ];
	pal = &wk->palRes[ ZKNSEARCHOBJ_PALRES_LOADING_BAR ];
	cel = &wk->celRes[ ZKNSEARCHOBJ_CELRES_LOADING_BAR ];
	*chr = GFL_CLGRP_CGR_Register(
					ah, NARC_zukan_gra_search_loading_NCGR,
					FALSE, CLSYS_DRAW_MAIN, HEAPID_ZUKAN_SEARCH );
  *pal = GFL_CLGRP_PLTT_Register(
					ah, NARC_zukan_gra_search_loading_NCLR,
					CLSYS_DRAW_MAIN, PALNUM_LOADING*0x20, HEAPID_ZUKAN_SEARCH );
	*cel = GFL_CLGRP_CELLANIM_Register(
					ah,
					NARC_zukan_gra_search_loading_NCER,
					NARC_zukan_gra_search_loading_NANR,
					HEAPID_ZUKAN_SEARCH );
	// �t�H�����n�a�i
	// ���C�����
	chr = &wk->chrRes[ ZKNSEARCHOBJ_CHRRES_FORM_M ];
	pal = &wk->palRes[ ZKNSEARCHOBJ_PALRES_FORM_M ];
	cel = &wk->celRes[ ZKNSEARCHOBJ_CELRES_FORM ];
	*chr = GFL_CLGRP_CGR_Register(
					ah, NARC_zukan_gra_search_zkn_form_NCGR,
					FALSE, CLSYS_DRAW_MAIN, HEAPID_ZUKAN_SEARCH );
  *pal = GFL_CLGRP_PLTT_Register(
					ah, NARC_zukan_gra_search_zkn_form_NCLR,
					CLSYS_DRAW_MAIN, PALNUM_FORM*0x20, HEAPID_ZUKAN_SEARCH );
	*cel = GFL_CLGRP_CELLANIM_Register(
					ah,
					NARC_zukan_gra_search_zkn_form_NCER,
					NARC_zukan_gra_search_zkn_form_NANR,
					HEAPID_ZUKAN_SEARCH );
	// �T�u���
	chr = &wk->chrRes[ ZKNSEARCHOBJ_CHRRES_FORM_S ];
	pal = &wk->palRes[ ZKNSEARCHOBJ_PALRES_FORM_S ];
	*chr = GFL_CLGRP_CGR_Register(
					ah, NARC_zukan_gra_search_zkn_form_NCGR,
					FALSE, CLSYS_DRAW_SUB, HEAPID_ZUKAN_SEARCH );
  *pal = GFL_CLGRP_PLTT_Register(
					ah, NARC_zukan_gra_search_zkn_form_NCLR,
					CLSYS_DRAW_SUB, PALNUM_FORM_S*0x20, HEAPID_ZUKAN_SEARCH );
	GFL_ARC_CloseDataHandle( ah );

	// �^�b�`�o�[
	ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_ZUKAN_SEARCH_L );
	chr = &wk->chrRes[ ZKNSEARCHOBJ_CHRRES_TOUCH_BAR ];
	pal = &wk->palRes[ ZKNSEARCHOBJ_PALRES_TOUCH_BAR ];
	cel = &wk->celRes[ ZKNSEARCHOBJ_CELRES_TOUCH_BAR ];
	*chr = GFL_CLGRP_CGR_Register(
					ah, APP_COMMON_GetBarIconCharArcIdx(),
					FALSE, CLSYS_DRAW_MAIN, HEAPID_ZUKAN_SEARCH );
  *pal = GFL_CLGRP_PLTT_Register(
					ah, APP_COMMON_GetBarIconPltArcIdx(),
					CLSYS_DRAW_MAIN, PALNUM_TOUCH_BAR*0x20,HEAPID_ZUKAN_SEARCH );
	*cel = GFL_CLGRP_CELLANIM_Register(
					ah,
					APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K),
					APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K),
					HEAPID_ZUKAN_SEARCH );
	GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���\�[�X���
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExitResource( ZKNSEARCHMAIN_WORK * wk )
{
	u32	i;

	for( i=0; i<ZKNSEARCHOBJ_CHRRES_MAX; i++ ){
		if( wk->chrRes[i] != RES_NONE ){
			GFL_CLGRP_CGR_Release( wk->chrRes[i] );
		}
	}
	for( i=0; i<ZKNSEARCHOBJ_PALRES_MAX; i++ ){
		if( wk->palRes[i] != RES_NONE ){
	    GFL_CLGRP_PLTT_Release( wk->palRes[i] );
		}
	}
	for( i=0; i<ZKNSEARCHOBJ_CELRES_MAX; i++ ){
		if( wk->celRes[i] != RES_NONE ){
	    GFL_CLGRP_CELLANIM_Release( wk->celRes[i] );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�ǉ�
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void AddClact( ZKNSEARCHMAIN_WORK * wk )
{
	u32	i;

	wk->clunit = GFL_CLACT_UNIT_Create( ZKNSEARCHOBJ_IDX_MAX, 0, HEAPID_ZUKAN_SEARCH );

	// ������
	for( i=0; i<ZKNSEARCHOBJ_IDX_MAX; i++ ){
		wk->clwk[i] = NULL;
	}

	for( i=0; i<=ZKNSEARCHOBJ_IDX_MARK2_SW; i++ ){
		ZKNCOMM_CLWK_DATA	dat = ClactParamTbl[i];
		dat.chrRes = wk->chrRes[dat.chrRes];
		dat.palRes = wk->palRes[dat.palRes];
		dat.celRes = wk->celRes[dat.celRes];
		wk->clwk[i] = ZKNCOMM_CreateClact( wk->clunit, &dat, HEAPID_ZUKAN_SEARCH );
	}
	// �x�o�^
	if( GAMEDATA_GetShortCut( wk->dat->gamedata, SHORTCUT_ID_ZUKAN_SEARCH ) == TRUE ){
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, APP_COMMON_BARICON_CHECK_ON );
	}

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_SCROLL_BAR, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_SCROLL_RAIL, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_LABEL, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_LOADING_BAR, FALSE );

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_M, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_M, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_S, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_S, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_SW, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_SW, FALSE );

	GFL_CLACT_WK_SetObjMode( wk->clwk[ZKNSEARCHOBJ_IDX_MARK1_SW], GX_OAM_MODE_OBJWND );
	GFL_CLACT_WK_SetObjMode( wk->clwk[ZKNSEARCHOBJ_IDX_MARK2_SW], GX_OAM_MODE_OBJWND );

	ZKNSEARCHOBJ_SetBlendMode( wk, ZKNSEARCHOBJ_IDX_MARK1_S, TRUE );
	ZKNSEARCHOBJ_SetBlendMode( wk, ZKNSEARCHOBJ_IDX_MARK2_S, TRUE );

	// �t�H����
	for( i=0; i<ZKNSEARCHOBJ_FORM_MAX; i++ ){
		ZKNCOMM_CLWK_DATA	dat = FormClactParamMain;
		dat.chrRes = wk->chrRes[dat.chrRes];
		dat.palRes = wk->palRes[dat.palRes];
		dat.celRes = wk->celRes[dat.celRes];
		dat.dat.anmseq = i;
		wk->clwk[ZKNSEARCHOBJ_IDX_FORM_M+i] = ZKNCOMM_CreateClact( wk->clunit, &dat, HEAPID_ZUKAN_SEARCH );
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_M+i, FALSE );

		dat = FormClactParamSub;
		dat.chrRes = wk->chrRes[dat.chrRes];
		dat.palRes = wk->palRes[dat.palRes];
		dat.celRes = wk->celRes[dat.celRes];
		dat.dat.anmseq = i;
		wk->clwk[ZKNSEARCHOBJ_IDX_FORM_S+i] = ZKNCOMM_CreateClact( wk->clunit, &dat, HEAPID_ZUKAN_SEARCH );
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_S+i, FALSE );
		ZKNSEARCHOBJ_SetBlendMode( wk, ZKNSEARCHOBJ_IDX_FORM_S+i, TRUE );

		wk->clwk[ZKNSEARCHOBJ_IDX_FORM_SW+i] = ZKNCOMM_CreateClact( wk->clunit, &dat, HEAPID_ZUKAN_SEARCH );
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_SW+i, FALSE );
		GFL_CLACT_WK_SetObjMode( wk->clwk[ZKNSEARCHOBJ_IDX_FORM_SW+i], GX_OAM_MODE_OBJWND );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�폜�i�ʁj
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		id			OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void DelClact( ZKNSEARCHMAIN_WORK * wk, u32 id )
{
	if( wk->clwk[id] != NULL ){
		GFL_CLACT_WK_Remove( wk->clwk[id] );
		wk->clwk[id] = NULL;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�폜�i�S�āj
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void DelClactAll( ZKNSEARCHMAIN_WORK * wk )
{
	u32	i;

	for( i=0; i<ZKNSEARCHOBJ_IDX_MAX; i++ ){
		DelClact( wk, i );
	}
	GFL_CLACT_UNIT_Delete( wk->clunit );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g��̂n�a�i���\���ɂ���
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_VanishList( ZKNSEARCHMAIN_WORK * wk )
{
	u32	i;

	// �X�N���[���o�[
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_SCROLL_BAR, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_SCROLL_RAIL, FALSE );

	// �}�[�N
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_M, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_M, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_S, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_S, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_SW, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_SW, FALSE );

	// �t�H����
	for( i=0; i<ZKNSEARCHOBJ_FORM_MAX; i++ ){
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_M+i, FALSE );
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_S+i, FALSE );
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_SW+i, FALSE );
	}
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_LABEL, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���y�[�W�̂n�a�i��\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_PutMainPage( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, FALSE );

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_EXIT, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, TRUE );

	if( wk->dat->sort->form != ZKNCOMM_LIST_SORT_NONE ){
		ZKNSEARCHOBJ_SetAnm( wk, ZKNSEARCHOBJ_IDX_FORM_LABEL, wk->dat->sort->form );
		ZKNSEARCHOBJ_SetPos(
			wk, ZKNSEARCHOBJ_IDX_FORM_LABEL,
			MAINPAGE_FORM_PX, MAINPAGE_FORM_PY, CLSYS_DRAW_MAIN );
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_LABEL, TRUE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ёI���y�[�W�̂n�a�i��\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_PutRowPage( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_EXIT, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, FALSE );

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���O�I���y�[�W�̂n�a�i��\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_PutNamePage( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_EXIT, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, FALSE );

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, TRUE );

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_SCROLL_BAR, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_SCROLL_RAIL, TRUE );

	ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_SCROLL_BAR, SCROLL_BAR_PX, SCROLL_BAR_UY, CLSYS_DRAW_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�C�v�I���y�[�W�̂n�a�i��\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_PutTypePage( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_EXIT, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, FALSE );

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, TRUE );

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_SCROLL_BAR, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_SCROLL_RAIL, TRUE );

	ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_SCROLL_BAR, SCROLL_BAR_PX, SCROLL_BAR_UY, CLSYS_DRAW_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�F�I���y�[�W�̂n�a�i��\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_PutColorPage( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_EXIT, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, FALSE );

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, TRUE );

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_SCROLL_BAR, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_SCROLL_RAIL, TRUE );

	ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_SCROLL_BAR, SCROLL_BAR_PX, SCROLL_BAR_UY, CLSYS_DRAW_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�H�����I���y�[�W�̂n�a�i��\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_PutFormPage( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_EXIT, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, FALSE );

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, TRUE );

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_SCROLL_BAR, TRUE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_SCROLL_RAIL, TRUE );

	ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_SCROLL_BAR, SCROLL_BAR_PX, SCROLL_BAR_UY, CLSYS_DRAW_MAIN );

	ZKNSEARCHOBJ_PutFormListNow( wk );

}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�H�����I���y�[�W�̑I������Ă���t�H������\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_PutFormListNow( ZKNSEARCHMAIN_WORK * wk )
{
	if( wk->dat->sort->form != ZKNCOMM_LIST_SORT_NONE ){
		ZKNSEARCHOBJ_SetAnm( wk, ZKNSEARCHOBJ_IDX_FORM_LABEL, wk->dat->sort->form );
		ZKNSEARCHOBJ_SetPos(
			wk, ZKNSEARCHOBJ_IDX_FORM_LABEL,
			FORMPAGE_FORM_PX, FORMPAGE_FORM_PY, CLSYS_DRAW_MAIN );
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_LABEL, TRUE );
	}else{
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_LABEL, FALSE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W���莞�̃t�H�����A�C�R���\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_VanishJumpFormIcon( ZKNSEARCHMAIN_WORK * wk )
{
	s16	i;
	s16	pos;

	pos = FRAMELIST_GetScrollCount( wk->lwk );

	for( i=0; i<ZKNSEARCHOBJ_FORM_MAX; i++ ){
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_M+i, FALSE );
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_S+i, FALSE );
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_SW+i, FALSE );
	}

	for( i=0; i<ZKNSEARCHLIST_FORMITEM_DISP_MAX+1; i++ ){
		ZKNSEARCHOBJ_PutFormList( wk, pos+i, MARK_SY_FORM*i, TRUE );
		if( ( pos - ( i + 1 ) ) >= 0 ){
			ZKNSEARCHOBJ_PutFormList( wk, pos-(i+1), 192-MARK_SY_FORM*(i+1), FALSE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�N���[���o�[�\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_SetScrollBar( ZKNSEARCHMAIN_WORK * wk )
{
	u32	py;
	s16	nx, ny;

	ZKNSEARCHOBJ_GetPos( wk, ZKNSEARCHOBJ_IDX_SCROLL_BAR, &nx, &ny, CLSYS_DRAW_MAIN );
	
	py = FRAMELIST_GetScrollBarPY2( wk->lwk, ny );

	if( py < SCROLL_BAR_UY ){
		py = SCROLL_BAR_UY;
	}else if( py > SCROLL_BAR_DY ){
		py = SCROLL_BAR_DY;
	}
	ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_SCROLL_BAR, SCROLL_BAR_PX, py, CLSYS_DRAW_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�؂�ւ����A�j���ݒ�
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		anm			FALSE = ��������
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_SetListPageArrowAnime( ZKNSEARCHMAIN_WORK * wk, BOOL anm )
{
	u32	pos = FRAMELIST_GetCursorPos( wk->lwk );

	if( ZKNSEARCHOBJ_GetAnm(wk,ZKNSEARCHOBJ_IDX_TB_LEFT) != APP_COMMON_BARICON_CURSOR_LEFT_ON ||
			anm == FALSE ){
		if( FRAMELIST_GetScrollCount( wk->lwk ) == 0 && pos == 0 ){
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT_OFF );
		}else{
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT );
		}
	}
	if( ZKNSEARCHOBJ_GetAnm(wk,ZKNSEARCHOBJ_IDX_TB_RIGHT) != APP_COMMON_BARICON_CURSOR_RIGHT_ON ||
			anm == FALSE ){
		if( FRAMELIST_CheckScrollMax( wk->lwk ) == FALSE && pos == (wk->listPosMax-1) ){
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT_OFF );
		}else{
			ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�`�F�b�N�}�[�N�\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		num			�}�[�N�ԍ�
 * @param		py			�\���x���W
 * @param		disp		�\�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_PutMark( ZKNSEARCHMAIN_WORK * wk, u16 num, s16 py, BOOL disp )
{
	if( disp == TRUE ){
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_M+num, TRUE );
		ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_MARK1_M+num, MARK_PX, py+MARK_PY, CLSYS_DRAW_MAIN );
	}else{
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_S+num, TRUE );
		ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_MARK1_S+num, MARK_PX, py+MARK_PY, CLSYS_DRAW_SUB );

		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_SW+num, TRUE );
		ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_MARK1_SW+num, MARK_PX, py+MARK_PY, CLSYS_DRAW_SUB );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�`�F�b�N�}�[�N��\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_VanishMark( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_M, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_M, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_S, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_S, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_SW, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_SW, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�`�F�b�N�}�[�N�؂�ւ�
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		pos			�ʒu
 * @param		flg			TRUE = �\��, FALSE = ��\��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_ChangeMark( ZKNSEARCHMAIN_WORK * wk, u16 pos, BOOL flg )
{
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_M, flg );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_S, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_SW, FALSE );
	if( flg == TRUE ){
		ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_MARK1_M, MARK_PX, MARK_PY+MARK_SY*pos, CLSYS_DRAW_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�`�F�b�N�}�[�N�؂�ւ��i�^�C�v�j
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		pos1		�ʒu�P
 * @param		pos2		�ʒu�Q
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_ChangeTypeMark( ZKNSEARCHMAIN_WORK * wk, u8	pos1, u8 pos2 )
{
	u8	list_pos = FRAMELIST_GetScrollCount( wk->lwk );

	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_M, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_M, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_S, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_S, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_SW, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_SW, FALSE );

	if( pos1 != ZKNCOMM_LIST_SORT_NONE ){
		u8	abs;
		pos1 = ZKNSEARCHMAIN_GetSortTypeIndex( wk, pos1 );
		abs  = GFL_STD_Abs( pos1 - list_pos );
		if( pos1 < list_pos ){
			if( abs <= 8 ){
				ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_S, TRUE );
				ZKNSEARCHOBJ_SetPos(
					wk, ZKNSEARCHOBJ_IDX_MARK1_S, MARK_PX, MARK_PY+MARK_SY*(8-abs), CLSYS_DRAW_SUB );
				ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_SW, TRUE );
				ZKNSEARCHOBJ_SetPos(
					wk, ZKNSEARCHOBJ_IDX_MARK1_SW, MARK_PX, MARK_PY+MARK_SY*(8-abs), CLSYS_DRAW_SUB );
			}
		}else{
			if( abs <= 7 ){
				ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_M, TRUE );
				ZKNSEARCHOBJ_SetPos(
					wk, ZKNSEARCHOBJ_IDX_MARK1_M, MARK_PX, MARK_PY+MARK_SY*abs, CLSYS_DRAW_MAIN );
			}
		}
	}

	if( pos2 != ZKNCOMM_LIST_SORT_NONE ){
		u8	abs;
		pos2 = ZKNSEARCHMAIN_GetSortTypeIndex( wk, pos2 );
		abs  = GFL_STD_Abs( pos2 - list_pos );
		if( pos2 < list_pos ){
			if( abs <= 8 ){
				ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_S, TRUE );
				ZKNSEARCHOBJ_SetPos(
					wk, ZKNSEARCHOBJ_IDX_MARK2_S, MARK_PX, MARK_PY+MARK_SY*(8-abs), CLSYS_DRAW_SUB );
				ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_SW, TRUE );
				ZKNSEARCHOBJ_SetPos(
					wk, ZKNSEARCHOBJ_IDX_MARK2_SW, MARK_PX, MARK_PY+MARK_SY*(8-abs), CLSYS_DRAW_SUB );
			}
		}else{
			if( abs <= 7 ){
				ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK2_M, TRUE );
				ZKNSEARCHOBJ_SetPos(
					wk, ZKNSEARCHOBJ_IDX_MARK2_M, MARK_PX, MARK_PY+MARK_SY*abs, CLSYS_DRAW_MAIN );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�`�F�b�N�}�[�W�\���i�t�H�����y�[�W�p�j
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		py			�\���x���W
 * @param		disp		�\�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_PutFormMark( ZKNSEARCHMAIN_WORK * wk, s16 py, BOOL disp )
{
	if( disp == TRUE ){
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_M, TRUE );
		ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_MARK1_M, MARK_PX, py+MARK_PY_FORM, CLSYS_DRAW_MAIN );
	}else{
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_S, TRUE );
		ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_MARK1_S, MARK_PX, py+MARK_PY_FORM, CLSYS_DRAW_SUB );

		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_SW, TRUE );
		ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_MARK1_SW, MARK_PX, py+MARK_PY_FORM, CLSYS_DRAW_SUB );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�`�F�b�N�}�[�N�؂�ւ��i�t�H�����y�[�W�p�j
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		pos			�ʒu
 * @param		flg			TRUE = �\��, FALSE = ��\��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_ChangeFormMark( ZKNSEARCHMAIN_WORK * wk, u16 pos, BOOL flg )
{
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_M, flg );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_S, FALSE );
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_SW, FALSE );
	if( flg == TRUE ){
		ZKNSEARCHOBJ_SetPos(
			wk, ZKNSEARCHOBJ_IDX_MARK1_M, MARK_PX_FORM, MARK_PY_FORM+MARK_SY_FORM*pos, CLSYS_DRAW_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W���莞�̃`�F�b�N�}�[�N�؂�ւ�
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		num			�}�[�N�ԍ�
 * @param		sel			�}�[�N�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_VanishJumpMark( ZKNSEARCHMAIN_WORK * wk, u16 num, u16 sel )
{
	u32	pos;
	u32	i;
	s16	start, end;

	pos = FRAMELIST_GetScrollCount( wk->lwk );

	start = pos - ZKNSEARCHLIST_ITEM_DISP_MAX;
	end   = pos + ZKNSEARCHLIST_ITEM_DISP_MAX;

	if( start < 0 ){ start = 0; }

	if( sel >= start && sel < pos ){
		ZKNSEARCHOBJ_PutMark( wk, num, (192)-MARK_SY*(pos-sel), FALSE );
	}else if( sel >= pos && sel < end ){
		ZKNSEARCHOBJ_PutMark( wk, num, MARK_SY*(sel-pos), TRUE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W���莞�̃`�F�b�N�}�[�N�؂�ւ��i�t�H�����y�[�W�j
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_VanishJumpMarkForm( ZKNSEARCHMAIN_WORK * wk )
{
	u32	pos;
	u32	i;
	s16	start, end;

	pos = FRAMELIST_GetScrollCount( wk->lwk );

	start = wk->dat->sort->form - ZKNSEARCHLIST_FORMITEM_DISP_MAX;
	end   = wk->dat->sort->form + ZKNSEARCHLIST_FORMITEM_DISP_MAX;

	if( start < 0 ){ start = 0; }

	if( wk->dat->sort->form >= start && wk->dat->sort->form < pos ){
		ZKNSEARCHOBJ_PutFormMark( wk, (192)-MARK_SY_FORM*(pos-wk->dat->sort->form), FALSE );
	}else if( wk->dat->sort->form >= pos && wk->dat->sort->form < end ){
		ZKNSEARCHOBJ_PutFormMark( wk, MARK_SY_FORM*(wk->dat->sort->form-pos), TRUE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g��Ƀt�H������\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		num			�t�H�����ԍ�
 * @param		py			�x���W
 * @param		disp		�\�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_PutFormList( ZKNSEARCHMAIN_WORK * wk, u16 num, s16 py, BOOL disp )
{
	if( disp == TRUE ){
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_M+num, TRUE );
		ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_FORM_M+num, FORM_PX, py+FORM_PY, CLSYS_DRAW_MAIN );
	}else{
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_S+num, TRUE );
		ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_FORM_S+num, FORM_PX, py+FORM_PY, CLSYS_DRAW_SUB );

		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_FORM_SW+num, TRUE );
		ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_FORM_SW+num, FORM_PX, py+FORM_PY, CLSYS_DRAW_SUB );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�X�N���[��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		mv			�X�N���[�����x
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_ScrollList( ZKNSEARCHMAIN_WORK * wk, s8 mv )
{
	u32	i;
	s16	x, y;

	for( i=ZKNSEARCHOBJ_IDX_MARK1_M; i<=ZKNSEARCHOBJ_IDX_MARK2_M; i++ ){
		if( ZKNSEARCHOBJ_CheckVanish( wk, i ) == FALSE ){
			continue;
		}
		ZKNSEARCHOBJ_GetPos( wk, i, &x, &y, CLSYS_DRAW_MAIN );
		ZKNSEARCHOBJ_SetPos( wk, i, x, y+mv, CLSYS_DRAW_MAIN );

		if( (y+mv) <= -12 || (y+mv) >= 204 ){
			ZKNSEARCHOBJ_SetVanish( wk, i, FALSE );
		}
	}
	for( i=ZKNSEARCHOBJ_IDX_MARK1_S; i<=ZKNSEARCHOBJ_IDX_MARK2_S; i++ ){
		if( ZKNSEARCHOBJ_CheckVanish( wk, i ) == FALSE ){
			continue;
		}
		ZKNSEARCHOBJ_GetPos( wk, i, &x, &y, CLSYS_DRAW_SUB );
		ZKNSEARCHOBJ_SetPos( wk, i, x, y+mv, CLSYS_DRAW_SUB );
		ZKNSEARCHOBJ_SetPos( wk, i+2, x, y+mv, CLSYS_DRAW_SUB );	// OBJ win

		if( (y+mv) <= -12 || (y+mv) >= 204 ){
			ZKNSEARCHOBJ_SetVanish( wk, i, FALSE );
			ZKNSEARCHOBJ_SetVanish( wk, i+2, FALSE );		// OBJ win
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�X�N���[���i�t�H�����y�[�W�p�j
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		mv			�X�N���[�����x
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_ScrollFormList( ZKNSEARCHMAIN_WORK * wk, s8 mv )
{
	u32	i;
	s16	x, y;

	for( i=ZKNSEARCHOBJ_IDX_FORM_M; i<ZKNSEARCHOBJ_IDX_FORM_M+ZKNSEARCHOBJ_FORM_MAX; i++ ){
		if( ZKNSEARCHOBJ_CheckVanish( wk, i ) == FALSE ){
			continue;
		}
		ZKNSEARCHOBJ_GetPos( wk, i, &x, &y, CLSYS_DRAW_MAIN );
		ZKNSEARCHOBJ_SetPos( wk, i, x, y+mv, CLSYS_DRAW_MAIN );

		if( (y+mv) <= -20 || (y+mv) >= 220 ){
			ZKNSEARCHOBJ_SetVanish( wk, i, FALSE );
		}
	}
	ZKNSEARCHOBJ_GetPos( wk, ZKNSEARCHOBJ_IDX_MARK1_M, &x, &y, CLSYS_DRAW_MAIN );
	ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_MARK1_M, x, y+mv, CLSYS_DRAW_MAIN );
	if( (y+mv) <= -20 || (y+mv) >= 220 ){
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_M, FALSE );
	}

	for( i=ZKNSEARCHOBJ_IDX_FORM_S; i<ZKNSEARCHOBJ_IDX_FORM_S+ZKNSEARCHOBJ_FORM_MAX; i++ ){
		if( ZKNSEARCHOBJ_CheckVanish( wk, i ) == FALSE ){
			continue;
		}
		ZKNSEARCHOBJ_GetPos( wk, i, &x, &y, CLSYS_DRAW_SUB );
		ZKNSEARCHOBJ_SetPos( wk, i, x, y+mv, CLSYS_DRAW_SUB );

		ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_FORM_MAX+i, x, y+mv, CLSYS_DRAW_SUB );

		if( (y+mv) <= -28 || (y+mv) >= 212 ){
			ZKNSEARCHOBJ_SetVanish( wk, i, FALSE );
			ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_FORM_MAX+i, FALSE );
		}
	}
	ZKNSEARCHOBJ_GetPos( wk, ZKNSEARCHOBJ_IDX_MARK1_S, &x, &y, CLSYS_DRAW_SUB );
	ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_MARK1_S, x, y+mv, CLSYS_DRAW_SUB );
	ZKNSEARCHOBJ_SetPos( wk, ZKNSEARCHOBJ_IDX_MARK1_SW, x, y+mv, CLSYS_DRAW_SUB );
	if( (y+mv) <= -28 || (y+mv) >= 212 ){
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_S, FALSE );
		ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_MARK1_SW, FALSE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�������o�[�\��
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		cnt			�\���J�E���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHOBJ_MoveLoadingBar( ZKNSEARCHMAIN_WORK * wk, u32 cnt )
{
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_LOADING_BAR, TRUE );
	ZKNSEARCHOBJ_SetPos(
		wk, ZKNSEARCHOBJ_IDX_LOADING_BAR,
		LOADING_BAR_PX+LOADING_BAR_MVX*cnt, LOADING_BAR_PY, CLSYS_DRAW_MAIN );
}
