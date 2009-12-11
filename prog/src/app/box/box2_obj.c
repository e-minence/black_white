//============================================================================================
/**
 * @file		box2_obj.c
 * @brief		�{�b�N�X��� OBJ�֘A
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 *
 *	���W���[�����FBOX2OBJ
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/main.h"
#include "system/poke2dgra.h"
#include "poke_tool/monsno_def.h"
#include "pokeicon/pokeicon.h"
#include "item/item.h"
#include "ui/touchbar.h"

#include "app/app_menu_common.h"
#include "app_menu_common.naix"

#include "box2_main.h"
#include "box2_obj.h"
#include "box2_bmp.h"
#include "box_gra.naix"


//============================================================================================
//	�萔��`
//============================================================================================

// ���C���p���b�g
#define	PALNUM_TRAYICON		( 0 )																		// �p���b�g�ԍ��F�g���C�A�C�R��
#define	PALSIZ_TRAYICON		( 3 )																		// �p���b�g���F�g���C�A�C�R��
//#define	PALNUM_WPICON			( PALNUM_TRAYICON + PALSIZ_TRAYICON )		// �p���b�g�ԍ��F�ǎ��A�C�R��
//#define	PALSIZ_WPICON			( 2 )																		// �p���b�g���F�ǎ��A�C�R��
//#define	PALNUM_POKEICON		( PALNUM_WPICON + PALSIZ_WPICON )				// �p���b�g�ԍ��F�|�P�����A�C�R��
#define	PALNUM_POKEICON		( PALNUM_TRAYICON + PALSIZ_TRAYICON )		// �p���b�g�ԍ��F�|�P�����A�C�R��
#define	PALSIZ_POKEICON		( POKEICON_PAL_MAX )										// �p���b�g���F�|�P�����A�C�R��
#define	PALNUM_BOXOBJ			( PALNUM_POKEICON + PALSIZ_POKEICON )		// �p���b�g�ԍ��F���̑��̂n�a�i
#define	PALSIZ_BOXOBJ			( 3 )																		// �p���b�g���F���̑��̂n�a�i
#define	PALNUM_TOUCH_BAR	( PALNUM_BOXOBJ + PALSIZ_BOXOBJ )				// �p���b�g�ԍ��F�^�b�`�o�[
#define	PALSIZ_TOUCH_BAR	( 4 )																		// �p���b�g���F�^�b�`�o�[
#define	PALNUM_ITEMICON		( PALNUM_TOUCH_BAR + PALSIZ_TOUCH_BAR )	// �p���b�g�ԍ��F�A�C�e���A�C�R��
#define	PALSIZ_ITEMICON		( 1 )																		// �p���b�g���F�A�C�e���A�C�R��

#define	PALNUM_OUTLINE		( 2 )										// �p���b�g�ԍ��F�A�E�g���C���J�[�\���i���\�[�X�̃I�t�Z�b�g�j
#define	PALNUM_TB_STATUS	( 3 )										// �p���b�g�ԍ��F�^�b�`�o�[�X�e�[�^�X�i���\�[�X�̃I�t�Z�b�g�j
#define	PLANUM_TRAYPOKE		( PALNUM_TRAYICON + 2 )		// �p���b�g�ԍ��F�g���C�A�C�R���ɕ\������h�b�g

// �T�u�p���b�g
#define	PALNUM_TYPEICON_S	( 0 )																			// �p���b�g�ԍ��F�^�C�v�A�C�R��
#define	PALSIZ_TYPEICON_S	( 3 )																			// �p���b�g���F�^�C�v�A�C�R��
#define	PALNUM_ITEMICON_S	( PALNUM_TYPEICON_S + PALSIZ_TYPEICON_S )	// �p���b�g�ԍ��F�A�C�e���A�C�R��
#define	PALSIZ_ITEMICON_S	( 1 )																			// �p���b�g���F�A�C�e���A�C�R��
#define	PALNUM_POKEGRA1_S	( PALNUM_ITEMICON_S + PALSIZ_ITEMICON_S )	// �p���b�g�ԍ��F�|�P�O���P
#define	PALSIZ_POKEGRA1_S	( 1 )																			// �p���b�g���F�|�P�O���P
#define	PALNUM_POKEGRA2_S	( PALNUM_POKEGRA1_S + PALSIZ_POKEGRA1_S )	// �p���b�g�ԍ��F�|�P�O���Q
#define	PALSIZ_POKEGRA2_S	( 1 )																			// �p���b�g���F�|�P�O���Q
#define	PALNUM_MARK_S			( PALNUM_POKEGRA2_S + PALSIZ_POKEGRA2_S )	// �p���b�g�ԍ��F�}�[�N
#define	PALSIZ_MARK_S			( 1 )																			// �p���b�g���F�}�[�N
#define	PALNUM_POKERUS_S	( PALNUM_MARK_S + PALSIZ_MARK_S )					// �p���b�g�ԍ��F�|�P���X�A�C�R��
#define	PALSIZ_POKERUS_S	( 1 )																			// �p���b�g���F�|�P���X�A�C�R��

/*
// �A�N�^�[�\���t���O
enum {
	ACT_OFF = 0,	// �A�N�^�[��\��
	ACT_ON			// �A�N�^�[�\��
};
*/

#define	BOX_SUB_ACTOR_DISTANCE	( 512*FX32_ONE )	// �T�u��ʂ�OBJ�\��Y���W�I�t�Z�b�g
#define	BOX_SUB_ACTOR_DEF_PY	( 320 )				// �T�u��ʂ�OBJ�\��Y���W�I�t�Z�b�g

/*
// �|�P�����ꖇ�G�f�[�^
#define	POKEGRA_TEX_SIZE	( 0x20 * 100 )	// �e�N�X�`���T�C�Y
#define	POKEGRA_PAL_SIZE	( 0x20 )		// �p���b�g�T�C�Y
*/
// �t�H���gOAM�Ŏg�p����OBJ�Ǘ���
// �g���C���Ǝ��[���Ŏg�p����BMPWIN�̃L�����T�C�Y�����ő�Ȃ̂ŁA���ꂾ������Α����B
#define	FNTOAM_CHR_MAX		( BOX2OBJ_FNTOAM_BOXNAME_SX*BOX2OBJ_FNTOAM_BOXNAME_SY*3 + BOX2OBJ_FNTOAM_BOXNUM_SX*BOX2OBJ_FNTOAM_BOXNUM_SY*6 )

// �|�P�����A�C�R���̃v���C�I���e�B�i80�͓K���j
#define	POKEICON_OBJ_PRI(a)			( 80 + BOX2OBJ_POKEICON_MAX - a*2 )	// �ʏ��OBJ�v���C�I���e�B
#define	POKEICON_OBJ_PRI_PUT(a)	( 80 + BOX2OBJ_POKEICON_MAX - a*2 )	// �z�u����OBJ�v���C�I���e�B
#define	POKEICON_OBJ_PRI_GET		( 20 )															// �擾����OBJ�v���C�I���e�B
#define	POKEICON_TBG_PRI_PUT		( 3 )																// �g���C�z�u����BG�v���C�I���e�B
#define	POKEICON_PBG_PRI_PUT		( 1 )																// �莝���z�u����BG�v���C�I���e�B
#define	POKEICON_BG_PRI_GET			( 0 )																// �擾����BG�v���C�I���e�B

#define	WP_IMAGE_COLOR_START	( 16 )		// �ǎ��̃C���[�W�J���[�J�n�ԍ�

#define	WP_ICON_CHG_COL			( 0x1e )	// �O���t�B�b�N�f�[�^�̕ǎ��̃f�t�H���g�J���[

#define	BOX2OBJ_TRAYICON_SY		( 34 )	// �g���C�A�C�R���̂x���W�z�u�Ԋu
#define	BOX2OBJ_TRAYICON_PX		( 300 )	// �g���C�A�C�R���̃f�t�H���g�w���W
#define	BOX2OBJ_TRAYICON_PY		( 0 )		// �g���C�A�C�R���̃f�t�H���g�x���W

#define	BOX2OBJ_TRAYNUM_PX		( 316 )	// �g���C�̊i�[���̃f�t�H���g�w���W
#define	BOX2OBJ_TRAYNUM_PY		( 2 )		// �g���C�̊i�[���̃f�t�H���g�x���W
#define	BOX2OBJ_TRAYNUM_SY		( 34 )	// �g���C�̊i�[���̂x���W�z�u�Ԋu

#define	BOX2OBJ_BOXNAME_DPX		( 36 )	// �{�b�N�X���̃f�t�H���g�w���W
#define	BOX2OBJ_BOXNAME_DPY		( 20 )	// �{�b�N�X���̃f�t�H���g�x���W
#define	BOX2OBJ_BOXNAME_RPX		( BOX2OBJ_BOXNAME_DPX+BOX2MAIN_TRAY_SCROLL_CNT*BOX2MAIN_TRAY_SCROLL_SPD )	// �{�b�N�X���̉E�z�u�w���W
#define	BOX2OBJ_BOXNAME_LPX		( BOX2OBJ_BOXNAME_DPX-BOX2MAIN_TRAY_SCROLL_CNT*BOX2MAIN_TRAY_SCROLL_SPD )	// �{�b�N�X���̍��z�u�w���W


#define	BOX2OBJ_WPICON_SX		( 46 )	// �ǎ��A�C�R���̂w���W�z�u�Ԋu
#define	BOX2OBJ_WPICON_PX		( 59 )	// �ǎ��A�C�R���̃f�t�H���g�w���W

#define	BOXNAME_OBJ_PX	( 212-6 )		// �g���C����OAM�t�H���g�̃f�t�H���g�\���w���W
#define	BOXNAME_OBJ_PY	( 18 )		// �g���C����OAM�t�H���g�̃f�t�H���g�\���x���W

#define	PARTYPOKE_FRM_PX	( 16 )		// �莝���|�P�����̃A�C�R���f�t�H���g�\���w���W
#define	PARTYPOKE_FRM_PY	( 192 )		// �莝���|�P�����̃A�C�R���f�t�H���g�\���x���W

// �g���C�̃|�P�����A�C�R���X�N���[���֘A
#define	TRAY_L_SCROLL_ICON_SX	( -8 )			// �A�C�R���\���J�n���W�i���j
#define	TRAY_L_SCROLL_ICON_EX	( 144+32 )		// �A�C�R���\���I�����W�i���j
#define	TRAY_R_SCROLL_ICON_SX	( 144+32 )		// �A�C�R���\���J�n���W�i�E�j
#define	TRAY_R_SCROLL_ICON_EX	( -8 )			// �A�C�R���\���I�����W�i�E�j

// �|�P�����𓦂����֘A
//#define	POKEICON_FREE_SCALE_DEF		( 1.0f )	// �f�t�H���g�g�k�l
//#define	POKEICON_FREE_SCALE_PRM		( 0.025f )	// �g�k�����l
#define	POKEICON_FREE_CNT_MAX			( 40 )				// �J�E���^
#define	POKEICON_FREE_SCALE_DEF		( FX32_ONE )	// �f�t�H���g�g�k�l
#define	POKEICON_FREE_SCALE_PRM		( FX32_ONE/POKEICON_FREE_CNT_MAX )		// �g�k�����l
#define	POKEICON_FREE_CY					( 8 )		// �g�k���S�x�I�t�Z�b�g

// �{�b�N�X�ړ��֘A
#define	BOXMV_LA_PX				( 12 )						// �����\���w���W
#define	BOXMV_LA_PY				( 27-48 )					// �����\���x���W
#define	BOXMV_RA_PX				( 244 )						// �E���\���w���W
#define	BOXMV_RA_PY				( 27-48 )					// �E���\���w���W
#define	BOXMV_TRAY_CUR_PX		(0)//( BOX2OBJ_TRAYICON_PX )		// �J�[�\���\���w���W
#define	BOXMV_TRAY_CUR_PY		( 27-48 )					// �J�[�\���\���x���W
#define	BOXMV_TRAY_NAME_PX		( 128 )						// �g���C���w�i�\���w���W
#define	BOXMV_TRAY_NAME_PY		( 8-48 )					// �g���C���w�i�\���w���W
#define	BOXMV_TRAY_ARROW_PX		( 128 )						// �J�[�\�����\���w���W
#define	BOXMV_TRAY_ARROW_PY		( 20-48 )					// �J�[�\�����\���x���W

#define	TRAY_ICON_CGX_SIZ		( 32 * 32 )		// �g���C�A�C�R���L�����T�C�Y
#define	TRAY_ICON_CHG_COL		( 8 )			// �g���C�A�C�R���ǎ��f�t�H���g�J���[

#define	TRAY_POKEDOT_PX		( 10 )		// �g���C�ɏ������݃|�P�����h�b�g�J�n�w���W
#define	TRAY_POKEDOT_PY		( 11 )		// �g���C�ɏ������݃|�P�����h�b�g�J�n�x���W
#define	TRAY_POKEDOT_SX		( 2 )		// �g���C�ɏ������݃|�P�����h�b�g�w�T�C�Y
#define	TRAY_POKEDOT_SY		( 2 )		// �g���C�ɏ������݃|�P�����h�b�g�x�T�C�Y

/*
// �ǎ��ύX�֘A
#define	WPCHG_LA_PX				( 13 )					// �����\���w���W
#define	WPCHG_LA_PY				( 27-48 )				// �����\���x���W
#define	WPCHG_RA_PX				( 243 )					// �E���\���w���W
#define	WPCHG_RA_PY				( 27-48 )				// �E���\���x���W
#define	WPCHG_TRAY_CUR_PX		( BOX2OBJ_WPICON_PX )	// �J�[�\���\���w���W
#define	WPCHG_TRAY_CUR_PY		( 27-48 )				// �J�[�\���\���x���W
#define	WPCHG_TRAY_NAME_PX		( 128 )					// ���O�w�i�\���w���W
#define	WPCHG_TRAY_NAME_PY		( 8-48 )				// ���O�w�i�\���x���W
#define WPCHG_TRAY_ARROW_PX		( 128 )					// �J�[�\�����\���w���W
#define	WPCHG_TRAY_ARROW_PY		( 20-48 )				// �J�[�\�����\���x���W
*/

#define	WP_ICON_CGX_SIZ		( 24 * 24 )		// �ǎ��A�C�R���L�����T�C�Y

// �|�P������a����{�b�N�X�I���֘A
#define	PTOUT_TRAY_ICON_PY		( 84 )						// �g���C�A�C�R���\���x���W
#define	PTOUT_LA_PX				( 12 )						// �g���C�؂�ւ������\���w���W
#define	PTOUT_LA_PY				( 84 )						// �g���C�؂�ւ������\���x���W
#define	PTOUT_RA_PX				( 244 )						// �g���C�؂�ւ��E���\���w���W
#define	PTOUT_RA_PY				( 84 )						// �g���C�؂�ւ��E���\���x���W
#define	PTOUT_TRAY_CUR_PX		(0)//( BOX2OBJ_TRAYICON_PX )		// �g���C�J�[�\���\���w���W
#define	PTOUT_TRAY_CUR_PY		( 84 )						// �g���C�J�[�\���\���x���W
#define	PTOUT_TRAY_NAME_PX		( 128 )						// �g���C���\���w���W
#define	PTOUT_TRAY_NAME_PY		( 65 )						// �g���C���\���x���W
#define	PTOUT_TRAY_ARROW_PX		( 128 )						// �J�[�\�����w���W
#define	PTOUT_TRAY_ARROW_PY		( 77 )						// �J�[�\�����x���W

// ���ʃA�C�e���A�C�R���\�����W
#define	SUBDISP_ITEMICON_PX	( 18 )
#define	SUBDISP_ITEMICON_PY	( 40 )

#define	RES_LOAD_NONE		( 0xffffffff )		// ���\�[�X��ǂݍ���łȂ��ꍇ




// �Z���A�N�^�[�f�[�^
typedef struct {
	GFL_CLWK_DATA	dat;

	u32	chrRes;
	u32	palRes;
	u32	celRes;

	u16	pal;
	u16	disp;

}BOX_CLWK_DATA;


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void ClactInit( BOX2_APP_WORK * appwk );
static void ClactResLoad( BOX2_APP_WORK * appwk );
static void ClactResFree( BOX2_APP_WORK * appwk );
static void ClactAdd( BOX2_SYS_WORK * syswk );
static void ClactDelAll( BOX2_APP_WORK * appwk );

static void BoxGraResLoad( BOX2_APP_WORK * appwk );
static void TouchBarGraResLoad( BOX2_APP_WORK * appwk );

static void PokeIconResLoad( BOX2_APP_WORK * appwk );
static void PokeIconObjAdd( BOX2_APP_WORK * appwk );

static void PokeGraDummyResLoad( BOX2_APP_WORK * appwk );

static void TypeIconObjAdd( BOX2_APP_WORK * appwk );
static void SubDispResLoad( BOX2_APP_WORK * appwk );

//static void WallPaperIconResLoad( BOX2_APP_WORK * appwk );
//static void WallPaperObjAdd( BOX2_APP_WORK * appwk );

static void ItemIconDummyResLoad( BOX2_APP_WORK * appwk );
//static void ItemIconObjAdd( BOX2_APP_WORK * appwk );

//static void BoxObjResLoad( BOX2_APP_WORK * appwk );
static void BoxObjAdd( BOX2_APP_WORK * appwk );

//static void TrayIconResLoad( BOX2_APP_WORK * appwk );
static void TrayObjAdd( BOX2_APP_WORK * appwk );

static void TrayPokeDotPut( BOX2_SYS_WORK * syswk, u32 tray, u8 * buf );

static void WallPaperBufferFill( BOX2_SYS_WORK * syswk, u8 * buf, u32 wp, u32 chg_col, u32 siz );


//============================================================================================
//	�O���[�o��
//============================================================================================

// �Z���A�N�^�[�f�[�^
static const BOX_CLWK_DATA ClactParamTbl[] =
{
	{	// �y�[�W���i���j
		{ 12, 28, BOX2OBJ_ANM_L_ARROW_OFF, 0, 3 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN
	},
	{	// �y�[�W���i�E�j
		{ 156, 28, BOX2OBJ_ANM_R_ARROW_OFF, 0, 3 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
/*
	{	// �{�b�N�X�ړ����i���j
		{ 12, -21, BOX2OBJ_ANM_L_ARROW_OFF, 5, 1 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
	{	// �{�b�N�X�ړ����i�E�j
		{ 244, -21, BOX2OBJ_ANM_R_ARROW_OFF, 5, 1 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
*/
	{	// �g���C�J�[�\��
		{ /*BOX2OBJ_TRAYICON_PX*/0, -21, BOX2OBJ_ANM_TRAY_CURSOR, 6, 1 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
	{	// �g���C���w�i
		{ 128, -40, BOX2OBJ_ANM_TRAY_NAME, 5, 1 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
/*
	{	// �g���C���
		{ BOXNAME_OBJ_PX, BOXNAME_OBJ_PY, BOX2OBJ_ANM_TRAY_ARROW, 5, 1 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
*/

	{	// ��J�[�\��
		{ 128, 128, BOX2OBJ_ANM_HAND_NORMAL, 0, 0 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
	{	// ��J�[�\���e
		{ 128, 160, BOX2OBJ_ANM_HAND_SHADOW, 0, 0 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},

	{	// �^�b�`�o�[�߂�{�^���i�{�b�N�X���j���[�ցj
		{ TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y, APP_COMMON_BARICON_RETURN, 64, 0 },
		BOX2MAIN_CHRRES_TOUCH_BAR, BOX2MAIN_PALRES_TOUCH_BAR, BOX2MAIN_CELRES_TOUCH_BAR,
		0, CLSYS_DRAW_MAIN,
	},
	{	// �^�b�`�o�[�I���{�^���iC-gear�ցj
		{ TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_Y, APP_COMMON_BARICON_EXIT, 64, 0 },
		BOX2MAIN_CHRRES_TOUCH_BAR, BOX2MAIN_PALRES_TOUCH_BAR, BOX2MAIN_CELRES_TOUCH_BAR,
		0, CLSYS_DRAW_MAIN,
	},
	{	// �^�b�`�o�[�X�e�[�^�X�{�^��
		{ TOUCHBAR_ICON_X_03, TOUCHBAR_ICON_Y, 0, 64, 0 },
		BOX2MAIN_CHRRES_BOX_BAR, BOX2MAIN_PALRES_TOUCH_BAR, BOX2MAIN_CELRES_BOX_BAR,
		PALNUM_TB_STATUS, CLSYS_DRAW_MAIN,
	},

	{	// �A�C�e���A�C�R��
		{ 0, 0, 0, 0, 0 },
		BOX2MAIN_CHRRES_ITEMICON, BOX2MAIN_PALRES_ITEMICON, BOX2MAIN_CELRES_ITEMICON,
		0, CLSYS_DRAW_MAIN,
	},
	{	// �A�C�e���A�C�R���i���ʁj
		{ 0, 0, 0, 0, 0 },
		BOX2MAIN_CHRRES_ITEMICON_SUB, BOX2MAIN_PALRES_ITEMICON, BOX2MAIN_CELRES_ITEMICON,
		0, CLSYS_DRAW_SUB,
	},

	{	// �|�P����
		{ 200, 56, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEGRA, BOX2MAIN_PALRES_POKEGRA, BOX2MAIN_CELRES_POKEGRA,
		0, CLSYS_DRAW_SUB,
	},
	{	// �|�P����
		{ 200, 56, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEGRA2, BOX2MAIN_PALRES_POKEGRA2, BOX2MAIN_CELRES_POKEGRA2,
		0, CLSYS_DRAW_SUB,
	},

/*
	{	// ���i���C���j
		{ 200, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK, BOX2MAIN_PALRES_POKEMARK, BOX2MAIN_CELRES_POKEMARK,
		0, CLSYS_DRAW_MAIN,
	},
	{	// ���i���C���j
		{ 208, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK, BOX2MAIN_PALRES_POKEMARK, BOX2MAIN_CELRES_POKEMARK,
		0, CLSYS_DRAW_MAIN,
	},
	{	// ���i���C���j
		{ 216, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK, BOX2MAIN_PALRES_POKEMARK, BOX2MAIN_CELRES_POKEMARK,
		0, CLSYS_DRAW_MAIN,
	},
	{	// ���i���C���j
		{ 224, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK, BOX2MAIN_PALRES_POKEMARK, BOX2MAIN_CELRES_POKEMARK,
		0, CLSYS_DRAW_MAIN,
	},
	{	// ���i���C���j
		{ 232, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK, BOX2MAIN_PALRES_POKEMARK, BOX2MAIN_CELRES_POKEMARK,
		0, CLSYS_DRAW_MAIN,
	},
	{	// �n�[�g�i���C���j
		{ 240, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK, BOX2MAIN_PALRES_POKEMARK, BOX2MAIN_CELRES_POKEMARK,
		0, CLSYS_DRAW_MAIN,
	},
*/

	{	// ���i�T�u�j
		{ 200, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// ���i�T�u�j
		{ 208, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// ���i�T�u�j
		{ 216, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// ���i�T�u�j
		{ 224, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// ���i�T�u�j
		{ 232, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// �n�[�g�i�T�u�j
		{ 240, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},

	{	// ���A
		{ 160, 104, APP_COMMON_POKE_MARK_STAR_RED, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// �|�P���X
		{ 168, 104, APP_COMMON_POKE_MARK_POKERUSU, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// �|�P���X�A�C�R��
		{ 116, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKERUSICON, BOX2MAIN_PALRES_POKERUSICON, BOX2MAIN_CELRES_POKERUSICON,
		0, CLSYS_DRAW_SUB,
	},
};

// �|�P�����A�C�R��
static const BOX_CLWK_DATA ClaPokeIconParam = {
	{ BOX2OBJ_TRAYPOKE_PX, BOX2OBJ_TRAYPOKE_PY, 0, 10, POKEICON_TBG_PRI_PUT },
	BOX2MAIN_CHRRES_POKEICON, BOX2MAIN_PALRES_POKEICON, BOX2MAIN_CELRES_POKEICON,
	0, CLSYS_DRAW_MAIN,
};

// �g���C�A�C�R��
static const BOX_CLWK_DATA ClaTrayIconParam = {
	{ BOX2OBJ_TRAYICON_PX, BOX2OBJ_TRAYICON_PY, 0, 10, 1 },
	BOX2MAIN_CHRRES_TRAYICON, BOX2MAIN_PALRES_TRAYICON, BOX2MAIN_CELRES_TRAYICON,
	0, CLSYS_DRAW_MAIN,
};

/*
// �ǎ��A�C�R��
static const BOX_CLWK_DATA ClaWallPaperIconParam = {
	{ BOX2OBJ_WPICON_PX, -21, 0, 10, 1 },
	BOX2MAIN_CHRRES_WPICON, BOX2MAIN_PALRES_WPICON, BOX2MAIN_CELRES_WPICON,
	0, CLSYS_DRAW_MAIN,
};
*/

// �^�C�v�A�C�R���i���ʁj
static const BOX_CLWK_DATA ClaTypeIconParam = {
	{ 160, 48, 0, 0, 1 },
	BOX2MAIN_CHRRES_TYPEICON, BOX2MAIN_PALRES_TYPEICON, BOX2MAIN_CELRES_TYPEICON,
	0, CLSYS_DRAW_SUB,
};


// �g���C�A�C�R���ɕ`�悷��|�P�����h�b�g�̃J���[
// �ǉ��ǎ��Ƌ��p�ł�
static const u8 TrayPokeDotColorPos[] = {
//  ��    ��    ��    ��    ��    ��    ��    �D    ��    ��
	0x0e, 0x0f, 0x05, 0x02, 0x0d, 0x0c, 0x06, 0x0b, 0x0a, 0x09,
};

static const s8	PokeCursorXTbl[] = { 1, -1, 0, 0, 1, 1, -1, -1 };	// �A�E�g���C���w���W�I�t�Z�b�g�e�[�u��
static const s8	PokeCursorYTbl[] = { 0, 0, 1, -1, 1, -1, 1, -1 };	// �A�E�g���C���x���W�I�t�Z�b�g�e�[�u��

// �莝���|�P�����̃A�C�R�����W�e�[�u��
static const s16 PartyPokeInitPos[6][2] =
{
	{ PARTYPOKE_FRM_PX+24, PARTYPOKE_FRM_PY+16 }, { PARTYPOKE_FRM_PX+64, PARTYPOKE_FRM_PY+24 },
	{ PARTYPOKE_FRM_PX+24, PARTYPOKE_FRM_PY+48 }, { PARTYPOKE_FRM_PX+64, PARTYPOKE_FRM_PY+56 },
	{ PARTYPOKE_FRM_PX+24, PARTYPOKE_FRM_PY+80 }, { PARTYPOKE_FRM_PX+64, PARTYPOKE_FRM_PY+88 },
};

//static const u8 BoxNamePosTbl[] = { 80, 96, 112, 144, 160, 176 };	// �{�b�N�X�ړ����̖��O�\���w���W�e�[�u��

// �莝���|�P�����\�����W
static const u8 PartyPokeFramePos[6][2] =
{
	{ 24, 16 }, { 64, 24 },
	{ 24, 48 }, { 64, 56 },
	{ 24, 80 }, { 64, 88 }
};




//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_Init( BOX2_SYS_WORK * syswk )
{
	ClactInit( syswk->app );
	ClactResLoad( syswk->app );
	ClactAdd( syswk );

	BOX2OBJ_SetTouchBarButton(
		syswk, syswk->tb_ret_btn, syswk->tb_exit_btn, syswk->tb_status_btn );

	BOX2OBJ_FontOamInit( syswk->app );

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// MAIN DISP OBJ ON
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// SUB DISP OBJ ON
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[���
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_Exit( BOX2_APP_WORK * appwk )
{
/*
	ClactResManExit( appwk );
*/
	BOX2OBJ_FontOamExit( appwk );

	ClactDelAll( appwk );
	ClactResFree( appwk );
	GFL_CLACT_SYS_Delete();
}

static void ClactInit( BOX2_APP_WORK * appwk )
{
	{
		const GFL_CLSYS_INIT init = {
			0, 0,									// ���C���@�T�[�t�F�[�X�̍�����W
			0, 512,								// �T�u�@�T�[�t�F�[�X�̍�����W
			4,										// ���C�����OAM�Ǘ��J�n�ʒu	4�̔{��
			124,									// ���C�����OAM�Ǘ���				4�̔{��
			4,										// �T�u���OAM�Ǘ��J�n�ʒu		4�̔{��
			124,									// �T�u���OAM�Ǘ���					4�̔{��
			0,										// �Z��Vram�]���Ǘ���

			BOX2MAIN_CHRRES_MAX+FNTOAM_CHR_MAX,	// �o�^�ł���L�����f�[�^��
			BOX2MAIN_PALRES_MAX,								// �o�^�ł���p���b�g�f�[�^��
			BOX2MAIN_CELRES_MAX+FNTOAM_CHR_MAX,	// �o�^�ł���Z���A�j���p�^�[����
			0,																	// �o�^�ł���}���`�Z���A�j���p�^�[�����i�����󖢑Ή��j

		  16,										// ���C�� CGR�@VRAM�Ǘ��̈�@�J�n�I�t�Z�b�g�i�L�����N�^�P�ʁj
		  16										//< �T�u CGR�@VRAM�Ǘ��̈�@�J�n�I�t�Z�b�g�i�L�����N�^�P�ʁj
		};
		GFL_CLACT_SYS_Create( &init, BOX2MAIN_GetVramBankData(), HEAPID_BOX_APP );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���\�[�X�ǂݍ���
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClactResLoad( BOX2_APP_WORK * appwk )
{
	BoxGraResLoad( appwk );
	TouchBarGraResLoad( appwk );
	PokeIconResLoad( appwk );
	ItemIconDummyResLoad( appwk );

	PokeGraDummyResLoad( appwk );
	SubDispResLoad( appwk );


/*
	TrayIconResLoad( appwk );
	WallPaperIconResLoad( appwk );
	PokeIconResLoad( appwk );
	BoxObjResLoad( appwk );
	ItemIconDummyResLoad( appwk );

//	PokeGraDummyResLoad( appwk );
	TypeIconResLoad( appwk );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���\�[�X�폜
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClactResFree( BOX2_APP_WORK * appwk )
{
	u32	i;

	for( i=0; i<BOX2MAIN_CHRRES_MAX; i++ ){
		if( appwk->chrRes[i] != RES_LOAD_NONE ){
			GFL_CLGRP_CGR_Release( appwk->chrRes[i] );
		}
	}
	for( i=0; i<BOX2MAIN_PALRES_MAX; i++ ){
		if( appwk->palRes[i] != RES_LOAD_NONE ){
	    GFL_CLGRP_PLTT_Release( appwk->palRes[i] );
		}
	}
	for( i=0; i<BOX2MAIN_CELRES_MAX; i++ ){
		if( appwk->celRes[i] != RES_LOAD_NONE ){
	    GFL_CLGRP_CELLANIM_Release( appwk->celRes[i] );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�ǉ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClactAdd( BOX2_SYS_WORK * syswk )
{
	syswk->app->clunit = GFL_CLACT_UNIT_Create( BOX2OBJ_ID_MAX+FNTOAM_CHR_MAX, 0, HEAPID_BOX_APP );

	TrayObjAdd( syswk->app );
//	WallPaperObjAdd( syswk->app );
	PokeIconObjAdd( syswk->app );
	BoxObjAdd( syswk->app );
	TypeIconObjAdd( syswk->app );

/*
	ItemIconObjAdd( syswk->app );

	PokeGraObjAdd( syswk );
*/
}

static GFL_CLWK * ClactWorkCreate( BOX2_APP_WORK * appwk, const BOX_CLWK_DATA * prm )
{
	GFL_CLWK * clwk;

	clwk = GFL_CLACT_WK_Create(
					appwk->clunit,
					appwk->chrRes[prm->chrRes],
					appwk->palRes[prm->palRes],
					appwk->celRes[prm->celRes],
					&prm->dat, prm->disp, HEAPID_BOX_APP );

//	GFL_CLACT_WK_SetPlttOffs( clwk, prm->pal, CLWK_PLTTOFFS_MODE_PLTT_TOP );

	return clwk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�폜
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		id			OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClactDel( BOX2_APP_WORK * appwk, u32 id )
{
	if( appwk->clwk[id] != NULL ){
		GFL_CLACT_WK_Remove( appwk->clwk[id] );
		appwk->clwk[id] = NULL;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�S�폜
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClactDelAll( BOX2_APP_WORK * appwk )
{
	u32	i;

	for( i=0; i<BOX2OBJ_ID_MAX; i++ ){
		ClactDel( appwk, i );
	}
	GFL_CLACT_UNIT_Delete( appwk->clunit );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j�����C��
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_AnmMain( BOX2_APP_WORK * appwk )
{
	u32	i;

	for( i=0; i<BOX2OBJ_ID_TRAYICON; i++ ){
		if( appwk->clwk[i] == NULL ){ continue; }
		if( GFL_CLACT_WK_GetAutoAnmFlag( appwk->clwk[i] ) == TRUE ){ continue; }
		GFL_CLACT_WK_AddAnmFrame( appwk->clwk[i], FX32_ONE );
	}
	GFL_CLACT_SYS_Main();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j���ύX
 *
 * @param		appwk	�{�b�N�X��ʃA�v�����[�N
 * @param		id		OBJ ID
 * @param		anm		�A�j���ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_AnmSet( BOX2_APP_WORK * appwk, u32 id, u32 anm )
{
	GFL_CLACT_WK_SetAnmFrame( appwk->clwk[id], 0 );
	GFL_CLACT_WK_SetAnmSeq( appwk->clwk[id], anm );
}

void BOX2OBJ_AutoAnmSet( BOX2_APP_WORK * appwk, u32 id, u32 anm )
{
	BOX2OBJ_AnmSet( appwk, id, anm );
	GFL_CLACT_WK_SetAutoAnmFlag( appwk->clwk[id], TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j���擾
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		id			OBJ ID
 *
 * @return	�A�j���ԍ�
 */
//--------------------------------------------------------------------------------------------
u32 BOX2OBJ_AnmGet( BOX2_APP_WORK * appwk, u32 id )
{
	return GFL_CLACT_WK_GetAnmSeq( appwk->clwk[id] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j����Ԏ擾
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		id			OBJ ID
 *
 * @retval	"TRUE = �A�j����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2OBJ_AnmCheck( BOX2_APP_WORK * appwk, u32 id )
{
	return GFL_CLACT_WK_CheckAnmActive( appwk->clwk[id] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�\���؂�ւ�
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		id			OBJ ID
 * @param		flg			�\���t���O
 *
 * @return	none
 *
 * @li	flg = TRUE : �\��
 * @li	flg = FALSE : ��\��
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_Vanish( BOX2_APP_WORK * appwk, u32 id, BOOL flg )
{
	if( appwk->clwk[id] != NULL ){
		GFL_CLACT_WK_SetDrawEnable( appwk->clwk[id], flg );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�\���`�F�b�N
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		id			OBJ ID
 *
 * @retval	"TRUE = �\��"
 * @retval	"FALSE = ��\��"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2OBJ_VanishCheck( BOX2_APP_WORK * appwk, u32 id )
{
	return GFL_CLACT_WK_GetDrawEnable( appwk->clwk[id] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[BG�v���C�I���e�B�ύX
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		id			OBJ ID
 * @param		pri			�v���C�I���e�B
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BgPriChange( BOX2_APP_WORK * appwk, u32 id, int pri )
{
	GFL_CLACT_WK_SetBgPri( appwk->clwk[id], pri );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[��OBJ���m�̃v���C�I���e�B�ύX
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		id			OBJ ID
 * @param		pri			�v���C�I���e�B
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ObjPriChange( BOX2_APP_WORK * appwk, u32 id, int pri )
{
//	appwk->obj_trans_stop = 1;
	GFL_CLACT_WK_SetSoftPri( appwk->clwk[id], pri );
//	appwk->obj_trans_stop = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�������ݒ�
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		id			OBJ ID
 * @param		flg			ON/OFF�t���O
 *
 * @return	none
 *
 * @li	flg : TRUE = ON, FALSE = OFF
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BlendModeSet( BOX2_APP_WORK * appwk, u32 id, BOOL flg )
{
	if( flg == TRUE ){
		GFL_CLACT_WK_SetObjMode( appwk->clwk[id], GX_OAM_MODE_XLU );
	}else{
		GFL_CLACT_WK_SetObjMode( appwk->clwk[id], GX_OAM_MODE_NORMAL );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[���W�ݒ�
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		id			OBJ ID
 * @param		x				�w���W
 * @param		y				�x���W
 * @param		setsf		�\�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_SetPos( BOX2_APP_WORK * appwk, u32 id, s16 x, s16 y, u16 setsf )
{
	GFL_CLACTPOS	pos;

	pos.x = x;
	pos.y = y;
	GFL_CLACT_WK_SetPos( appwk->clwk[id], &pos, setsf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[���W�擾
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		id			OBJ ID
 * @param		x				�w���W
 * @param		y				�x���W
 * @param		setsf		�\�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_GetPos( BOX2_APP_WORK * appwk, u32 id, s16 * x, s16 * y, u16 setsf )
{
	GFL_CLACTPOS	pos;

	GFL_CLACT_WK_GetPos( appwk->clwk[id], &pos, setsf );
	*x = pos.x;
	*y = pos.y;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�p���b�g�ύX
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		id			OBJ ID
 * @param		pal			�p���b�g�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ChgPltt( BOX2_APP_WORK * appwk, u32 id, u32 pal )
{
	GFL_CLACT_WK_SetPlttOffs( appwk->clwk[id], pal, CLWK_PLTTOFFS_MODE_PLTT_TOP );
}





static void BoxGraResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
	u32	i;

	ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	// �g���C�A�C�R��
	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_TRAYICON+i] = GFL_CLGRP_CGR_Register(
																									ah, NARC_box_gra_box_tray_lz_NCGR,
																									TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	}
  appwk->palRes[BOX2MAIN_PALRES_TRAYICON] = GFL_CLGRP_PLTT_Register(
																							ah, NARC_box_gra_box_tray_NCLR,
																							CLSYS_DRAW_MAIN, PALNUM_TRAYICON*0x20, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_TRAYICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_box_tray_NCER,
																							NARC_box_gra_box_tray_NANR,
																							HEAPID_BOX_APP );

/*
	// �ǎ��A�C�R��
	for( i=0; i<BOX2OBJ_WPICON_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_WPICON+i] = GFL_CLGRP_CGR_Register(
																								ah, NARC_box_gra_box_wp_image_NCGR,
																								FALSE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	}
	appwk->celRes[BOX2MAIN_CELRES_WPICON] = GFL_CLGRP_CELLANIM_Register(
																						ah,
																						NARC_box_gra_box_wp_image_NCER,
																						NARC_box_gra_box_wp_image_NANR,
																						HEAPID_BOX_APP );
  appwk->palRes[BOX2MAIN_PALRES_WPICON] = GFL_CLGRP_PLTT_Register(
																						ah, NARC_box_gra_box_wp_image_NCLR,
																						CLSYS_DRAW_MAIN, PALNUM_WPICON*0x20, HEAPID_BOX_APP );
*/

	// �|�P�����A�C�R��
	// �L����
	for( i=0; i<BOX2OBJ_POKEICON_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_POKEICON+i] = GFL_CLGRP_CGR_Register(
																									ah, NARC_box_gra_box_pokeicon_lz_NCGR,
																									TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	}

	// ���̑��̂n�a�i
	appwk->chrRes[BOX2MAIN_CHRRES_BOXOBJ] = GFL_CLGRP_CGR_Register(
																						ah, NARC_box_gra_box_m_obj_lz_NCGR,
																						TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
  appwk->palRes[BOX2MAIN_PALRES_BOXOBJ] = GFL_CLGRP_PLTT_Register(
																						ah, NARC_box_gra_box_m_obj2_NCLR,
																						CLSYS_DRAW_MAIN, PALNUM_BOXOBJ*0x20, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_BOXOBJ] = GFL_CLGRP_CELLANIM_Register(
																						ah,
																						NARC_box_gra_box_m_obj_NCER,
																						NARC_box_gra_box_m_obj_NANR,
																						HEAPID_BOX_APP );

	// �X�e�[�^�X�{�^��
	appwk->chrRes[BOX2MAIN_CHRRES_BOX_BAR] = GFL_CLGRP_CGR_Register(
																							ah, NARC_box_gra_bar_button_lz_NCGR,
																							TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_BOX_BAR] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_bar_button_NCER,
																							NARC_box_gra_bar_button_NANR,
																							HEAPID_BOX_APP );

	// �A�C�e���A�C�R��
	appwk->chrRes[BOX2MAIN_CHRRES_ITEMICON] = GFL_CLGRP_CGR_Register(
																							ah, NARC_box_gra_box_itemicon_lz_NCGR,
																							TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_ITEMICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_box_itemicon_NCER,
																							NARC_box_gra_box_itemicon_NANR,
																							HEAPID_BOX_APP );
	appwk->chrRes[BOX2MAIN_CHRRES_ITEMICON_SUB] = GFL_CLGRP_CGR_Register(
																									ah, NARC_box_gra_box_itemicon_lz_NCGR,
																									TRUE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );

/*
	// �^�C�v�A�C�R��
	appwk->chrRes[BOX2MAIN_CHRRES_TYPEICON1] = GFL_CLGRP_CGR_Register(
																							ah, NARC_box_gra_box_typeicon_lz_NCGR,
																							TRUE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	appwk->chrRes[BOX2MAIN_CHRRES_TYPEICON2] = GFL_CLGRP_CGR_Register(
																							ah, NARC_box_gra_box_typeicon_lz_NCGR,
																							TRUE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_TYPEICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_box_typeicon_NCER,
																							NARC_box_gra_box_typeicon_NANR,
																							HEAPID_BOX_APP );
*/

  GFL_ARC_CloseDataHandle( ah );
}

static void TouchBarGraResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
	u32	i;

	ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_TOUCH_BAR] = GFL_CLGRP_CGR_Register(
																							ah, APP_COMMON_GetBarIconCharArcIdx(),
																							FALSE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
  appwk->palRes[BOX2MAIN_PALRES_TOUCH_BAR] = GFL_CLGRP_PLTT_Register(
																							ah, APP_COMMON_GetBarIconPltArcIdx(),
																							CLSYS_DRAW_MAIN, PALNUM_TOUCH_BAR*0x20, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_TOUCH_BAR] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K),
																							APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K),
																							HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}


//============================================================================================
//	�|�P�����A�C�R��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R�����\�[�X�ǂݍ���
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 *
 *	�L�����̓_�~�[
 */
//--------------------------------------------------------------------------------------------
static void PokeIconResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
/*
	u32	i;

	ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	// �L����
	for( i=0; i<BOX2OBJ_POKEICON_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_POKEICON+i] = GFL_CLGRP_CGR_Register(
																									ah, NARC_box_gra_box_pokeicon_lz_NCGR,
																									TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	}

  GFL_ARC_CloseDataHandle( ah );
*/

	ah = GFL_ARC_OpenDataHandle( ARCID_POKEICON, HEAPID_BOX_APP );

	// �p���b�g
  appwk->palRes[BOX2MAIN_PALRES_POKEICON] = GFL_CLGRP_PLTT_RegisterComp(
																							ah, POKEICON_GetPalArcIndex(),
																							CLSYS_DRAW_MAIN, PALNUM_POKEICON*0x20, HEAPID_BOX_APP );

	// �Z���E�A�j��
  appwk->celRes[BOX2MAIN_CELRES_POKEICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							POKEICON_GetCellArcIndex(),
																							POKEICON_GetAnmArcIndex(),
																							HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R��OBJ�ǉ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
*/
//--------------------------------------------------------------------------------------------
static void PokeIconObjAdd( BOX2_APP_WORK * appwk )
{
	BOX_CLWK_DATA	prm;
	u32	i;

	for( i=0; i<BOX2OBJ_POKEICON_MAX; i++ ){
		prm = ClaPokeIconParam;
		prm.dat.pos_x += ( i % BOX2OBJ_POKEICON_H_MAX * BOX2OBJ_TRAYPOKE_SX );
		prm.dat.pos_y += ( i / BOX2OBJ_POKEICON_H_MAX * BOX2OBJ_TRAYPOKE_SY );
		prm.dat.softpri = POKEICON_OBJ_PRI(i);
		prm.chrRes = BOX2MAIN_CHRRES_POKEICON + i;
		appwk->clwk[BOX2OBJ_ID_POKEICON+i] = ClactWorkCreate( appwk, &prm );

		appwk->pokeicon_id[i] = BOX2OBJ_ID_POKEICON+i;
	}

/*
	TCATS_OBJECT_ADD_PARAM_S	prm;
	u32	i;

	for( i=0; i<BOX2OBJ_POKEICON_MAX; i++ ){
		prm = ClaPokeIconParam;
		prm.x += ( i % BOX2OBJ_POKEICON_H_MAX * BOX2OBJ_TRAYPOKE_SX );
		prm.y += ( i / BOX2OBJ_POKEICON_H_MAX * BOX2OBJ_TRAYPOKE_SY );
		prm.pri = POKEICON_OBJ_PRI(i);
		prm.id[0] = CHR_ID_POKEICON + i;

		appwk->cap[BOX2OBJ_ID_POKEICON+i] = CATS_ObjectAdd_S( appwk->csp, appwk->crp, &prm );

		appwk->pokeicon_id[i] = BOX2OBJ_ID_POKEICON+i;
	}
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �n�a�i�Ɋ��蓖�Ă��Ă���L�����f�[�^������������
 *
 * @param	appwk		�{�b�N�X��ʃA�v�����[�N
 * @param	id			OBJ ID
 * @param	buf			�L�����f�[�^
 * @param	siz			�L�����T�C�Y
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void CgxChangeTrans( BOX2_APP_WORK * appwk, u32 id, u8 * buf, u32 siz )
{
	NNSG2dImageProxy * ipc;
	u32	cgx;

	ipc = CLACT_ImageProxyGet( appwk->cap[id]->act );
	cgx = NNS_G2dGetImageLocation( ipc, NNS_G2D_VRAM_TYPE_2DMAIN );

	DC_FlushRange( buf, siz );
	GX_LoadOBJ( buf, cgx, siz );
}
*/


//--------------------------------------------------------------------------------------------
/**
 * �n�a�i�Ɋ��蓖�Ă��Ă���L�����f�[�^������������i�T�u��ʗp�j
 *
 * @param	appwk		�{�b�N�X��ʃA�v�����[�N
 * @param	id			OBJ ID
 * @param	buf			�L�����f�[�^
 * @param	siz			�L�����T�C�Y
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void CgxChangeTransSub( BOX2_APP_WORK * appwk, u32 id, u8 * buf, u32 siz )
{
	NNSG2dImageProxy * ipc;
	u32	cgx;

	ipc = CLACT_ImageProxyGet( appwk->cap[id]->act );
	cgx = NNS_G2dGetImageLocation( ipc, NNS_G2D_VRAM_TYPE_2DSUB );

	DC_FlushRange( buf, siz );
	GXS_LoadOBJ( buf, cgx, siz );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���L�����f�[�^�ǂݍ���
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	ppp		POKEMON_PASO_PARAM
 * @param	chr		NNSG2dCharacterData
 *
 * @return	buf
 */
//--------------------------------------------------------------------------------------------
static void * PokeIconCgxLoad(
				BOX2_APP_WORK * appwk, const POKEMON_PASO_PARAM * ppp, NNSG2dCharacterData ** chr )
{
	void * buf;
	u32	arc;

	arc = POKEICON_GetCgxArcIndex( ppp );
	buf = GFL_ARCHDL_UTIL_LoadOBJCharacter( appwk->pokeicon_ah, arc, FALSE, chr, HEAPID_BOX_APP );

	return buf;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���̃L�����f�[�^�����[�N�ɓW�J
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	tray	�g���C�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconBufLoad( BOX2_SYS_WORK * syswk, u32 tray )
{
	POKEMON_PASO_PARAM * ppp;
	NNSG2dCharacterData * chr;
	void * buf;
	u32	i;

	for( i=0; i<BOX2OBJ_POKEICON_TRAY_MAX; i++ ){
		// �|�P���������݂��Ȃ�
		if( BOXDAT_PokeParaGet( syswk->dat->sv_box, tray, i, ID_PARA_poke_exist, NULL ) == 0 ){
			syswk->app->pokeicon_exist[i] = FALSE;
			continue;
		}
		// �擾�ʒu
		if( syswk->dat->callMode != BOX_MODE_ITEM && syswk->tray == syswk->get_tray && syswk->get_pos == i ){
			syswk->app->pokeicon_exist[i] = FALSE;
			continue;
		}
		ppp = BOX2MAIN_PPPGet( syswk, tray, i );
		buf = PokeIconCgxLoad( syswk->app, ppp, &chr );
		GFL_STD_MemCopy32( chr->pRawData, syswk->app->pokeicon_cgx[i], BOX2OBJ_POKEICON_CGX_SIZE );
		GFL_HEAP_FreeMemory( buf );

		syswk->app->pokeicon_pal[i] = POKEICON_GetPalNumGetByPPP( ppp );

		syswk->app->pokeicon_exist[i] = TRUE;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���̃L�����f�[�^��ύX
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	buf		�L�����f�[�^�o�b�t�@
 * @param	id		OBJ ID
 * @param	pal		�p���b�g�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconChangeCore( BOX2_APP_WORK * appwk, void * buf, u32 id, u32 pal )
{
//	CgxChangeTrans( appwk, id, buf, BOX2OBJ_POKEICON_CGX_SIZE );
	GFL_CLGRP_CGR_ReplaceEx(
		GFL_CLACT_WK_GetCgrIndex( appwk->clwk[id] ),
		buf, BOX2OBJ_POKEICON_CGX_SIZE, 0, CLSYS_DRAW_MAIN );

	GFL_CLACT_WK_SetPlttOffs( appwk->clwk[id], pal, CLWK_PLTTOFFS_MODE_PLTT_TOP );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���̃L�����f�[�^��ύX
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	ppp		POKEMON_PASO_PARAM
 * @param	id		OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconChange( BOX2_SYS_WORK * syswk, const POKEMON_PASO_PARAM * ppp, u32 id )
{
	NNSG2dCharacterData * chr;
	void * buf;

	buf = PokeIconCgxLoad( syswk->app, ppp, &chr );

	PokeIconChangeCore( syswk->app, chr->pRawData, id, POKEICON_GetPalNumGetByPPP(ppp) );

	GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����A�C�R���ύX
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		tray		�g���C�ԍ�
 * @param		pos			�|�P�����ʒu
 * @param		id			OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconChange( BOX2_SYS_WORK * syswk, u32 tray, u32 pos, u32 id )
{
	BOX2_APP_WORK * appwk = syswk->app;

	BOX2OBJ_Vanish( appwk, id, FALSE );

	if( BOX2MAIN_PokeParaGet( syswk, pos, syswk->tray, ID_PARA_poke_exist, NULL ) == 0 ){
		return;
	}

	PokeIconChange( syswk, BOX2MAIN_PPPGet( syswk, tray, pos ), id );
	BOX2OBJ_Vanish( appwk, id, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�̃|�P�����A�C�R����ύX
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayPokeIconChange( BOX2_SYS_WORK * syswk )
{
	u32	i;

	for( i=0; i<BOX2OBJ_POKEICON_TRAY_MAX; i++ ){
		BOX2OBJ_PokeIconChange( syswk, syswk->tray, i, BOX2OBJ_ID_POKEICON+i );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�莝���̃|�P�����A�C�R����ύX
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconChange( BOX2_SYS_WORK * syswk )
{
	const POKEMON_PASO_PARAM * ppp;
	u32	ppcnt;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

	for( i=0; i<ppcnt; i++ ){
		ppp = PP_GetPPPPointerConst( PokeParty_GetMemberPointer(syswk->dat->pokeparty,i) );
		PokeIconChange( syswk, ppp, syswk->app->pokeicon_id[BOX2OBJ_POKEICON_TRAY_MAX+i] );
		BOX2OBJ_Vanish( syswk->app, syswk->app->pokeicon_id[BOX2OBJ_POKEICON_TRAY_MAX+i], FALSE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���̃f�t�H���g���W�擾
 *
 * @param	pos		�ʒu
 * @param	x		�w���W�i�[�ꏊ
 * @param	y		�x���W�i�[�ꏊ
 * @param	mode	�|�P�����A�C�R���ړ����[�h
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconDefaultPosGet( u32 pos, s16 * x, s16 * y, u32 mode )
{
	if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		*x = BOX2OBJ_TRAYPOKE_PX + ( pos % BOX2OBJ_POKEICON_H_MAX * BOX2OBJ_TRAYPOKE_SX );
		*y = BOX2OBJ_TRAYPOKE_PY + ( pos / BOX2OBJ_POKEICON_H_MAX * BOX2OBJ_TRAYPOKE_SY );
	}else{
		// �莝��
		pos -= BOX2OBJ_POKEICON_TRAY_MAX;
		*x = PartyPokeInitPos[pos][0];
		*y = PartyPokeInitPos[pos][1] - BOX2MAIN_PARTYPOKE_FRM_SPD * BOX2MAIN_PARTYPOKE_FRM_CNT;
		if( mode == BOX2MAIN_POKEMOVE_MODE_ALL ){
			*x = *x + 152;
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���Ƀf�t�H���g���W��ݒ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	pos		�ʒu
 * @param	id		OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconDefaultPosSet( BOX2_SYS_WORK * syswk, u32 pos, u32 objID )
{
	s16	x, y;

	BOX2OBJ_PokeIconDefaultPosGet( pos, &x, &y, syswk->move_mode );
	BOX2OBJ_SetPos( syswk->app, objID, x, y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_ObjPriChange( syswk->app, objID, POKEICON_OBJ_PRI(pos) );
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����A�C�R�����t���[���C���O�̍��W�Ɉړ��i���j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconFrmInSet( BOX2_SYS_WORK * syswk )
{
	u32	ppcnt;
	u32	id;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_TRAY_MAX+i ];
		BOX2OBJ_SetPos(
			syswk->app, id, PartyPokeInitPos[i][0], PartyPokeInitPos[i][1], CLSYS_DEFREND_MAIN );
		BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_TRAY_MAX+i, BOX2OBJ_POKEICON_PRI_CHG_PUT );
		if( i < ppcnt ){
			BOX2OBJ_Vanish( syswk->app, id, TRUE );
		}else{
			BOX2OBJ_Vanish( syswk->app, id, FALSE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����A�C�R�����t���[���C���O�̍��W�Ɉړ��i�E�j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconFrmInSetRight( BOX2_SYS_WORK * syswk )
{
	u32	ppcnt;
	u32	id;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_TRAY_MAX+i ];
		BOX2OBJ_SetPos(
			syswk->app, id,
			PartyPokeInitPos[i][0]+BOX2MAIN_PARTYPOKE_FRM_H_SPD*BOX2MAIN_PARTYPOKE_FRM_H_CNT,
			PartyPokeInitPos[i][1],
			CLSYS_DEFREND_MAIN );
		BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_TRAY_MAX+i, BOX2OBJ_POKEICON_PRI_CHG_PUT );
		if( i < ppcnt ){
			BOX2OBJ_Vanish( syswk->app, id, TRUE );
		}else{
			BOX2OBJ_Vanish( syswk->app, id, FALSE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����A�C�R�����t���[���C����̍��W�Ɉړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconFrmSet( BOX2_SYS_WORK * syswk )
{
	u32	ppcnt;
	u32	id;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_TRAY_MAX+i ];
		BOX2OBJ_SetPos(
			syswk->app, id,
			PartyPokeInitPos[i][0],
			PartyPokeInitPos[i][1] - BOX2MAIN_PARTYPOKE_FRM_SPD * BOX2MAIN_PARTYPOKE_FRM_CNT,
			CLSYS_DEFREND_MAIN );
		BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_TRAY_MAX+i, BOX2OBJ_POKEICON_PRI_CHG_PUT );
		if( i < ppcnt ){
			BOX2OBJ_Vanish( syswk->app, id, TRUE );
		}else{
			BOX2OBJ_Vanish( syswk->app, id, FALSE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���|�P�����A�C�R�����t���[���C����̍��W�Ɉړ��i�E�j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconFrmSetRight( BOX2_SYS_WORK * syswk )
{
	u32	ppcnt;
	u32	id;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_TRAY_MAX+i ];
		BOX2OBJ_SetPos(
			syswk->app, id,
			PartyPokeInitPos[i][0] + BOX2MAIN_PARTYPOKE_FRM_H_SPD * BOX2MAIN_PARTYPOKE_FRM_H_CNT,
			PartyPokeInitPos[i][1] - BOX2MAIN_PARTYPOKE_FRM_SPD * BOX2MAIN_PARTYPOKE_FRM_CNT,
			CLSYS_DEFREND_MAIN );
		BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_TRAY_MAX+i, BOX2OBJ_POKEICON_PRI_CHG_PUT );
		if( i < ppcnt ){
			BOX2OBJ_Vanish( syswk->app, id, TRUE );
		}else{
			BOX2OBJ_Vanish( syswk->app, id, FALSE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���̃v���C�I���e�B��ύX
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	pos		�ʒu
 * @param	flg		�擾���z�u��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconPriChg( BOX2_APP_WORK * appwk, u32 pos, u32 flg )
{
	u32	id = appwk->pokeicon_id[pos];

	if( flg == BOX2OBJ_POKEICON_PRI_CHG_GET ){
		BOX2OBJ_BgPriChange( appwk, id, POKEICON_BG_PRI_GET );
		BOX2OBJ_ObjPriChange( appwk, id, POKEICON_OBJ_PRI_GET );
	}else{
		if( pos < BOX2OBJ_POKEICON_TRAY_MAX || pos == BOX2OBJ_POKEICON_GET_POS ){
			BOX2OBJ_BgPriChange( appwk, id, POKEICON_TBG_PRI_PUT );
		}else{
			BOX2OBJ_BgPriChange( appwk, id, POKEICON_PBG_PRI_PUT );
		}
		BOX2OBJ_ObjPriChange( appwk, id, POKEICON_OBJ_PRI_PUT(pos) );
	}
}

void BOX2OBJ_PokeIconPriChg2( BOX2_APP_WORK * appwk, u32 icon_pos, u32 put_pos, u32 flg )
{
	u32	id = appwk->pokeicon_id[icon_pos];

	if( flg == BOX2OBJ_POKEICON_PRI_CHG_GET ){
		BOX2OBJ_BgPriChange( appwk, id, POKEICON_BG_PRI_GET );
		BOX2OBJ_ObjPriChange( appwk, id, POKEICON_OBJ_PRI_GET );
	}else{
		if( put_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2OBJ_BgPriChange( appwk, id, POKEICON_TBG_PRI_PUT );
		}else{
			BOX2OBJ_BgPriChange( appwk, id, POKEICON_PBG_PRI_PUT );
		}
		BOX2OBJ_ObjPriChange( appwk, id, POKEICON_OBJ_PRI_PUT(put_pos) );
	}
}


//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���̃v���C�I���e�B��ύX�i�͂񂾎莝�����g���C�ɗ��Ƃ��Ƃ��j
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	dp		�͂񂾈ʒu
 * @param	mp		�z�u����ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconPriChgDrop( BOX2_APP_WORK * appwk, u32 dp, u32 mp )
{
	u32	id = appwk->pokeicon_id[dp];

	if( mp < BOX2OBJ_POKEICON_TRAY_MAX ){
		BOX2OBJ_BgPriChange( appwk, id, POKEICON_TBG_PRI_PUT );
	}else{
		BOX2OBJ_BgPriChange( appwk, id, POKEICON_PBG_PRI_PUT );
	}
	BOX2OBJ_ObjPriChange( appwk, id, POKEICON_OBJ_PRI_GET );
}


//--------------------------------------------------------------------------------------------
/**
 * �g���C�̃|�P�����A�C�R���X�N���[������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	mv		�ړ���
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayPokeIconScroll( BOX2_SYS_WORK * syswk, s8 mv )
{
	s16	x, y;
	s16	sp, ep;
	u16	id;
	u16	i;

	if( mv >= 0 ){
		ep = TRAY_L_SCROLL_ICON_EX;
		sp = TRAY_L_SCROLL_ICON_SX;
	}else{
		ep = TRAY_R_SCROLL_ICON_EX;
		sp = TRAY_R_SCROLL_ICON_SX;
	}

	for( i=0; i<BOX2OBJ_POKEICON_TRAY_MAX; i++ ){
		id = syswk->app->pokeicon_id[i];
		BOX2OBJ_GetPos( syswk->app, id, &x, &y, CLSYS_DEFREND_MAIN );
		x += mv;
		if( x == ep ){
			x = sp;
			PokeIconChangeCore(
				syswk->app, syswk->app->pokeicon_cgx[i], id, syswk->app->pokeicon_pal[i] );
			BOX2OBJ_Vanish( syswk->app, id, syswk->app->pokeicon_exist[i] );
			if( syswk->dat->callMode == BOX_MODE_ITEM ){
				BOX2OBJ_PokeIconBlendSetItem( syswk, i );
			}
		}
		BOX2OBJ_SetPos( syswk->app, id, x, y, CLSYS_DEFREND_MAIN );

/*
		{
			if( i % 6 == 0 ){
				OS_Printf( "\n" );
			}
			BOX2OBJ_GetPos( syswk->app, id, &x, &y, CLSYS_DEFREND_MAIN );
			OS_Printf( "[%d], %d, %d, %d", i, x, y, syswk->app->pokeicon_exist[i] );
		}
*/

	}
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���̃|�P�����A�C�R���X�N���[������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	mx		�ړ��ʂw
 * @param	my		�ړ��ʂx
 *
 * @return	none
 *
 *	���g�p���Ă���Ƃ��떳��
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconScroll( BOX2_SYS_WORK * syswk, s8 mx, s8 my )
{
	s16	x, y;
	u16	id;
	u16	i;

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[BOX2OBJ_POKEICON_TRAY_MAX+i];
		BOX2OBJ_GetPos( syswk->app, id, &x, &y, CLSYS_DEFREND_MAIN );
		x += mx;
		y += my;
		BOX2OBJ_SetPos( syswk->app, id, x, y, CLSYS_DEFREND_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �莝���̃|�P�����A�C�R���X�N���[������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconScroll2( BOX2_SYS_WORK * syswk )
{
	u16	id;
	u16	i;
	s16	px, py;
	s8	wfrm_x, wfrm_y;

	BGWINFRM_PosGet( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY, &wfrm_x, &wfrm_y );
	px = wfrm_x * 8;
	py = wfrm_y * 8;

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[BOX2OBJ_POKEICON_TRAY_MAX+i];
		BOX2OBJ_SetPos(
			syswk->app, id,
			px + PartyPokeFramePos[i][0],
			py + PartyPokeFramePos[i][1],
			CLSYS_DEFREND_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����𓦂����F�|�P�����A�C�R�����쏉����
 *
 * @param	wk		���������[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconFreeStart( BOX2MAIN_POKEFREE_WORK * wk )
{
	GFL_CLSCALE	scale;
	GFL_CLACTPOS	pos;

	wk->scale     = POKEICON_FREE_SCALE_DEF;
	wk->scale_cnt = 0;

	GFL_CLACT_WK_SetAffineParam( wk->clwk, CLSYS_AFFINETYPE_NORMAL );

	scale.x = wk->scale;
	scale.y = wk->scale;
	GFL_CLACT_WK_SetScale( wk->clwk, &scale );

	pos.x = 0;
	pos.y = POKEICON_FREE_CY;
	GFL_CLACT_WK_SetAffinePos( wk->clwk, &pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����𓦂����F�|�P�����A�C�R���k������
 *
 * @param	wk		���������[�N
 *
 * @retval	"TRUE = ������"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2OBJ_PokeIconFreeMove( BOX2MAIN_POKEFREE_WORK * wk )
{
	GFL_CLSCALE	scale;

	wk->scale_cnt++;
	wk->scale -= POKEICON_FREE_SCALE_PRM;

	if( wk->scale_cnt == POKEICON_FREE_CNT_MAX ){
		return FALSE;
	}

	scale.x = wk->scale;
	scale.y = wk->scale;
	GFL_CLACT_WK_SetScale( wk->clwk, &scale );

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����𓦂����F�|�P�����A�C�R���g�又��
 *
 * @param	wk		���������[�N
 *
 * @retval	"TRUE = ������"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2OBJ_PokeIconFreeErrorMove( BOX2MAIN_POKEFREE_WORK * wk )
{
	GFL_CLSCALE	scale;

	wk->scale_cnt -= 2;
	wk->scale += POKEICON_FREE_SCALE_PRM;
	wk->scale += POKEICON_FREE_SCALE_PRM;

	if( wk->scale_cnt == 0 ){
		return FALSE;
	}

	scale.x = wk->scale;
	scale.y = wk->scale;
	GFL_CLACT_WK_SetScale( wk->clwk, &scale );

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����𓦂����F�|�P�����A�C�R���I���ݒ�
 *
 * @param	wk		���������[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconFreeEnd( BOX2MAIN_POKEFREE_WORK * wk )
{
	GFL_CLACT_WK_SetDrawEnable( wk->clwk, FALSE );
	BOX2OBJ_PokeIconFreeErrorEnd( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����𓦂����F�|�P�����A�C�R���G���[�I���ݒ�
 *
 * @param	wk		���������[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconFreeErrorEnd( BOX2MAIN_POKEFREE_WORK * wk )
{
	GFL_CLSCALE	scale;
	GFL_CLACTPOS	pos;

	pos.x = 0;
	pos.y = 0;
	GFL_CLACT_WK_SetAffinePos( wk->clwk, &pos );

	scale.x = POKEICON_FREE_SCALE_DEF;
	scale.y = POKEICON_FREE_SCALE_DEF;
	GFL_CLACT_WK_SetScale( wk->clwk, &scale );

	GFL_CLACT_WK_SetAffineParam( wk->clwk, CLSYS_AFFINETYPE_NONE );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���ɔ������ݒ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	flg		ON/OFF�t���O
 * @param	start	�J�n�A�C�R���ʒu
 * @param	end		�I���A�C�R���ʒu
 * @param	item	�������ɉe������邩
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconBlendSetCore( BOX2_SYS_WORK * syswk, BOOL flg, u32 start, u32 end, BOOL item )
{
	u32	i;

	if( item == TRUE ){
		for( i=start; i<end; i++ ){
			if( BOX2MAIN_PokeParaGet( syswk, i, syswk->tray, ID_PARA_item, NULL ) == 0 ){
				BOX2OBJ_BlendModeSet( syswk->app, syswk->app->pokeicon_id[i], TRUE );
			}else{
				BOX2OBJ_BlendModeSet( syswk->app, syswk->app->pokeicon_id[i], FALSE );
			}
		}
	}else{
		for( i=start; i<end; i++ ){
			BOX2OBJ_BlendModeSet( syswk->app, syswk->app->pokeicon_id[i], flg );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���ɔ������ݒ�i�S�́j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	type	�A�C�R���ʒu
 * @param	flg		ON/OFF�t���O
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconBlendSetAll( BOX2_SYS_WORK * syswk, u32 type, BOOL flg )
{
	BOOL item;

	if( type & BOX2OBJ_BLENDTYPE_ITEM ){
		item = TRUE;
	}else{
		item = FALSE;
	}

	if( ( type & BOX2OBJ_BLENDTYPE_TRAYPOKE	) != 0 ){
		PokeIconBlendSetCore( syswk, flg, 0, BOX2OBJ_POKEICON_TRAY_MAX, item );
	}
	if( ( type & BOX2OBJ_BLENDTYPE_PARTYPOKE ) != 0 ){
		PokeIconBlendSetCore( syswk, flg, BOX2OBJ_POKEICON_TRAY_MAX, BOX2OBJ_POKEICON_MAX, item );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���ɔ������ݒ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	pos		�A�C�R���ʒu
 * @param	flg		ON/OFF�t���O
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconBlendSet( BOX2_APP_WORK * appwk, u32 pos, BOOL flg )
{
	BOX2OBJ_BlendModeSet( appwk, appwk->pokeicon_id[pos], flg );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����A�C�R���ɔ������ݒ�i�����������̃g���C�X�N���[���p�j
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	pos		�A�C�R���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconBlendSetItem( BOX2_SYS_WORK * syswk, u32 pos )
{
	PokeIconBlendSetCore( syswk, TRUE, pos, pos+1, TRUE );
}

// �|�P�A�C�R�����擾�����Ƃ��̂n�a�i���ꂩ��
void BOX2OBJ_GetPokeIcon( BOX2_APP_WORK * appwk, u32 pos )
{
	s16	x, y;
	u8	tmp;

	BOX2OBJ_GetPos( appwk, appwk->pokeicon_id[pos], &x, &y, CLSYS_DEFREND_MAIN );

	tmp = appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS];
	appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS] = appwk->pokeicon_id[pos];
	appwk->pokeicon_id[pos] = tmp;

//	OS_Printf( "get = %d, put = %d\n", appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS],appwk->pokeicon_id[pos] );

	BOX2OBJ_SetPos( appwk, appwk->pokeicon_id[pos], x, y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_PokeIconPriChg( appwk, pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
}

// �|�P�A�C�R����z�u�����Ƃ��̂n�a�i���ꂩ��
void BOX2OBJ_PutPokeIcon( BOX2_APP_WORK * appwk, u32 pos )
{
	s16	x, y;
	u8	tmp;

	BOX2OBJ_GetPos( appwk, appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS], &x, &y, CLSYS_DEFREND_MAIN );

	tmp = appwk->pokeicon_id[pos];
	appwk->pokeicon_id[pos] = appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS];
	appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS] = tmp;

	BOX2OBJ_SetPos( appwk, appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS], x, y, CLSYS_DEFREND_MAIN );
//	BOX2OBJ_PokeIconPriChg( appwk, pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
/*
	if( pos < BOX2OBJ_POKEICON_TRAY_MAX || pos == BOX2OBJ_POKEICON_GET_POS ){
		BOX2OBJ_BgPriChange( appwk, appwk->pokeicon_id[pos], POKEICON_TBG_PRI_PUT );
	}else{
		BOX2OBJ_BgPriChange( appwk, appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS], POKEICON_PBG_PRI_PUT );
	}
	BOX2OBJ_ObjPriChange( appwk, appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS], POKEICON_OBJ_PRI_PUT(pos) );
*/
}


// �擾��Ԃ̈ړ�
void BOX2OBJ_MovePokeIconHand( BOX2_SYS_WORK * syswk )
{
	u32	id;
	s16	x, y;

	if( syswk->poke_get_key == 0 ){
		return;
	}

	BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, &x, &y, CLSYS_DEFREND_MAIN );

	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		id = BOX2OBJ_ID_ITEMICON;
		y += 8;
	}else{
		id = syswk->app->pokeicon_id[BOX2OBJ_POKEICON_GET_POS];
		y += 4;
	}

	BOX2OBJ_SetPos( syswk->app, id, x, y, CLSYS_DEFREND_MAIN );
}


//============================================================================================
//	�|�P�����O���t�B�b�N
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����O���t�B�b�N���\�[�X�ǂݍ���
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeGraDummyResLoad( BOX2_APP_WORK * appwk )
{
	appwk->chrRes[BOX2MAIN_CHRRES_POKEGRA] = RES_LOAD_NONE;
	appwk->palRes[BOX2MAIN_PALRES_POKEGRA] = RES_LOAD_NONE;
	appwk->celRes[BOX2MAIN_CELRES_POKEGRA] = RES_LOAD_NONE;

	appwk->chrRes[BOX2MAIN_CHRRES_POKEGRA2] = RES_LOAD_NONE;
	appwk->palRes[BOX2MAIN_PALRES_POKEGRA2] = RES_LOAD_NONE;
	appwk->celRes[BOX2MAIN_CELRES_POKEGRA2] = RES_LOAD_NONE;

/*
	CATS_LoadResourceCharArc(
		appwk->csp, appwk->crp,
		ARC_BATT_OBJ, POKE_OAM_NCGR, 0, NNS_G2D_VRAM_TYPE_2DSUB, CHR_ID_POKEGRA );
	CATS_LoadResourceCharArc(
		appwk->csp, appwk->crp,
		ARC_BATT_OBJ, POKE_OAM_NCGR, 0, NNS_G2D_VRAM_TYPE_2DSUB, CHR_ID_POKEGRA2 );

	CATS_LoadResourcePlttArc(
		appwk->csp, appwk->crp,
		ARC_BATT_OBJ, POKE_OAM_NCLR, 0, 1, NNS_G2D_VRAM_TYPE_2DSUB, PAL_ID_POKEGRA );
	CATS_LoadResourcePlttArc(
		appwk->csp, appwk->crp,
		ARC_BATT_OBJ, POKE_OAM_NCLR, 0, 1, NNS_G2D_VRAM_TYPE_2DSUB, PAL_ID_POKEGRA2 );

	CATS_LoadResourceCellArc(
		appwk->csp, appwk->crp, ARC_BATT_OBJ, POKE_OAM_NCER, 0, CEL_ID_POKEGRA );
	CATS_LoadResourceCellAnmArc(
		appwk->csp, appwk->crp, ARC_BATT_OBJ, POKE_OAM_NANR, 0, ANM_ID_POKEGRA );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����O���t�B�b�NOBJ�ǉ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void PokeGraObjAdd( BOX2_SYS_WORK * syswk )
{
	BOX2_APP_WORK * appwk = syswk->app;

	appwk->cap[BOX2OBJ_ID_POKEGRA]  = CATS_ObjectAdd_S_SubDistance(
										appwk->csp, appwk->crp,
										&ClactParamTbl[BOX2OBJ_ID_POKEGRA],
										BOX_SUB_ACTOR_DISTANCE );
	appwk->cap[BOX2OBJ_ID_POKEGRA2] = CATS_ObjectAdd_S_SubDistance(
										appwk->csp, appwk->crp,
										&ClactParamTbl[BOX2OBJ_ID_POKEGRA2],
										BOX_SUB_ACTOR_DISTANCE );

	GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_POKEGRA], ACT_OFF );
	GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_POKEGRA2], ACT_OFF );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����O���t�B�b�N�؂�ւ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	info	�\���f�[�^
 * @param	id		OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeGraChange( BOX2_APP_WORK * appwk, BOX2_POKEINFO_DATA * info, u32 id )
{
/*
	SOFT_SPRITE_ARC  ssa;
	void * buf;
	NNSG2dImageProxy * ipc;
	NNSG2dImagePaletteProxy * ipp;
	u32	cgx;
	u32	pal;
	CHANGES_INTO_DATA_RECT rect = { POKE_TEX_X, POKE_TEX_Y, POKE_TEX_W, POKE_TEX_H, };

	buf = GFL_HEAP_AllocMemoryLo( HEAPID_BOX_APP, POKEGRA_TEX_SIZE );

	PokeGraArcDataGetPPP( &ssa, info->ppp, PARA_FRONT, FALSE );

	Ex_ChangesInto_OAM_from_PokeTex_RC(
		ssa.arc_no,
		ssa.index_chr,
		HEAPID_BOX_APP,
		&rect,
		buf,
		info->rand,
		FALSE,
		PARA_FRONT,
		info->mons );

	ipc = CLACT_ImageProxyGet( appwk->cap[id]->act );
	cgx = NNS_G2dGetImageLocation( ipc, NNS_G2D_VRAM_TYPE_2DSUB );

	DC_FlushRange( buf, POKEGRA_TEX_SIZE );
	GXS_LoadOBJ( buf, cgx, POKEGRA_TEX_SIZE );

	ipp = GFL_CLACT_WK_GetPlttProxy( appwk->cap[id]->act );
	pal = NNS_G2dGetImagePaletteLocation( ipp, NNS_G2D_VRAM_TYPE_2DSUB );

	GFL_ARC_UTIL_TransVramPalette(
		ssa.arc_no, ssa.index_pal, PALTYPE_SUB_OBJ, pal, POKEGRA_PAL_SIZE, HEAPID_BOX_APP );

	GFL_HEAP_FreeMemory( buf );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����O���t�B�b�N�؂�ւ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	info	�\���f�[�^
 * @param	id		OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeGraChange( BOX2_SYS_WORK * syswk, BOX2_POKEINFO_DATA * info, u32 id )
{
/*
	id += syswk->app->pokegra_swap;

	PokeGraChange( syswk->app, info, id );
	GFL_CLACT_UNIT_SetDrawEnableCap( syswk->app->cap[id], ACT_ON );

	syswk->app->pokegra_swap ^= 1;
*/


/*
//�|�P�O���̃A�[�N�n���h��
//�����X�^�[�ԍ�������ǂޏꍇ
extern u32 POKE2DGRA_OBJ_PLTT_Register( ARCHANDLE *p_handle, int mons_no, int form_no, int sex, int rare, int dir, CLSYS_DRAW_TYPE vramType, u16 byteOffs, HEAPID heapID );
extern u32 POKE2DGRA_OBJ_CGR_Register( ARCHANDLE *p_handle, int mons_no, int form_no, int sex, int rare, int dir, CLSYS_DRAW_TYPE vramType, HEAPID heapID );
extern u32 POKE2DGRA_OBJ_CELLANM_Register( int mons_no, int form_no, int sex, int rare, int dir, APP_COMMON_MAPPING mapping, CLSYS_DRAW_TYPE vramType, HEAPID heapID );
*/

/*
	ARCHANDLE * ah;
	u32 * chrRes;
	u32 * palRes;
	u32 * celRes;
	u16	pal;

	if( syswk->app->clwk[BOX2OBJ_ID_POKEGRA] != NULL ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_POKEGRA, FALSE );
	}
	if( syswk->app->clwk[BOX2OBJ_ID_POKEGRA2] != NULL ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_POKEGRA2, FALSE );
	}

	if( syswk->app->pokegra_swap == 0 ){
		chrRes = &syswk->app->chrRes[BOX2MAIN_CHRRES_POKEGRA];
		palRes = &syswk->app->palRes[BOX2MAIN_PALRES_POKEGRA];
		celRes = &syswk->app->celRes[BOX2MAIN_CELRES_POKEGRA];
		pal = PALNUM_POKEGRA1_S * 0x20;
	}else{
		chrRes = &syswk->app->chrRes[BOX2MAIN_CHRRES_POKEGRA2];
		palRes = &syswk->app->palRes[BOX2MAIN_PALRES_POKEGRA2];
		celRes = &syswk->app->celRes[BOX2MAIN_CELRES_POKEGRA2];
		pal = PALNUM_POKEGRA2_S * 0x20;
		id += 1;
	}

	if( syswk->app->clwk[id] != NULL ){
		GFL_CLACT_WK_Remove( syswk->app->clwk[id] );
		GFL_CLGRP_CGR_Release( *chrRes );
    GFL_CLGRP_PLTT_Release( *palRes );
    GFL_CLGRP_CELLANIM_Release( *celRes );
	}

	ah = POKE2DGRA_OpenHandle( HEAPID_BOX_APP );

	// �t�H�����ƃ��A���K�v�I
	*chrRes = POKE2DGRA_OBJ_CGR_Register( ah, info->mons, 0, info->sex, 0, POKEGRA_DIR_FRONT, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	*palRes = POKE2DGRA_OBJ_PLTT_Register( ah, info->mons, 0, info->sex, 0, POKEGRA_DIR_FRONT, CLSYS_DRAW_SUB, pal, HEAPID_BOX_APP );
	*celRes = POKE2DGRA_OBJ_CELLANM_Register( info->mons, 0, info->sex, 0, POKEGRA_DIR_FRONT, APP_COMMON_MAPPING_128K, CLSYS_DRAW_SUB, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );

	syswk->app->clwk[id] = ClactWorkCreate( syswk->app, &ClactParamTbl[id] );
	BOX2OBJ_Vanish( syswk->app, id, TRUE );

	syswk->app->pokegra_swap ^= 1;
*/
	ARCHANDLE * ah;
	u32 * chrRes;
	u32 * palRes;
	u32 * celRes;
	u32	oldID;
	u16	pal;
	BOOL	fast;

	if( syswk->app->pokegra_swap == 0 ){
		chrRes = &syswk->app->chrRes[BOX2MAIN_CHRRES_POKEGRA];
		palRes = &syswk->app->palRes[BOX2MAIN_PALRES_POKEGRA];
		celRes = &syswk->app->celRes[BOX2MAIN_CELRES_POKEGRA];
		pal = PALNUM_POKEGRA1_S * 0x20;
		oldID = id + 1;
	}else{
		chrRes = &syswk->app->chrRes[BOX2MAIN_CHRRES_POKEGRA2];
		palRes = &syswk->app->palRes[BOX2MAIN_PALRES_POKEGRA2];
		celRes = &syswk->app->celRes[BOX2MAIN_CELRES_POKEGRA2];
		pal = PALNUM_POKEGRA2_S * 0x20;
		oldID = id;
		id += 1;
	}

	if( syswk->app->clwk[id] != NULL ){
		GFL_CLACT_WK_Remove( syswk->app->clwk[id] );
		GFL_CLGRP_CGR_Release( *chrRes );
    GFL_CLGRP_PLTT_Release( *palRes );
    GFL_CLGRP_CELLANIM_Release( *celRes );
	}

	ah = POKE2DGRA_OpenHandle( HEAPID_BOX_APP );

	fast = PPP_FastModeOn( info->ppp );
	*chrRes = POKE2DGRA_OBJ_CGR_RegisterPPP( ah, info->ppp, POKEGRA_DIR_FRONT, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	*palRes = POKE2DGRA_OBJ_PLTT_RegisterPPP( ah, info->ppp, POKEGRA_DIR_FRONT, CLSYS_DRAW_SUB, pal, HEAPID_BOX_APP );
	*celRes = POKE2DGRA_OBJ_CELLANM_RegisterPPP( info->ppp, POKEGRA_DIR_FRONT, APP_COMMON_MAPPING_128K, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	PPP_FastModeOff( info->ppp, fast );

  GFL_ARC_CloseDataHandle( ah );

	syswk->app->clwk[id] = ClactWorkCreate( syswk->app, &ClactParamTbl[id] );
	BOX2OBJ_Vanish( syswk->app, id, TRUE );

	if( syswk->app->clwk[oldID] != NULL ){
		BOX2OBJ_Vanish( syswk->app, oldID, FALSE );
	}

	syswk->app->pokegra_swap ^= 1;
}


//============================================================================================
//	�^�C�v�A�C�R��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �^�C�v�A�C�R�����\�[�X�ǂݍ���
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SubDispResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
	u32	i;

	ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_BOX_APP_L );

	for( i=0; i<POKETYPE_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_TYPEICON+i] = GFL_CLGRP_CGR_Register(
																									ah,
																									APP_COMMON_GetPokeTypeCharArcIdx(i),
																									FALSE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	}

	appwk->celRes[BOX2MAIN_CELRES_TYPEICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							APP_COMMON_GetPokeTypeCellArcIdx(APP_COMMON_MAPPING_128K),
																							APP_COMMON_GetPokeTypeAnimeArcIdx(APP_COMMON_MAPPING_128K),
																							HEAPID_BOX_APP );

  appwk->palRes[BOX2MAIN_PALRES_TYPEICON] = GFL_CLGRP_PLTT_Register(
																							ah, APP_COMMON_GetPokeTypePltArcIdx(),
																							CLSYS_DRAW_SUB, PALNUM_TYPEICON_S*0x20, HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_POKEMARK_SUB] = GFL_CLGRP_CGR_Register(
																									ah,
																									APP_COMMON_GetPokeMarkCharArcIdx(APP_COMMON_MAPPING_128K),
																									FALSE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );

	appwk->celRes[BOX2MAIN_CELRES_POKEMARK_SUB] = GFL_CLGRP_CELLANIM_Register(
																									ah,
																									APP_COMMON_GetPokeMarkCellArcIdx(APP_COMMON_MAPPING_128K),
																									APP_COMMON_GetPokeMarkAnimeArcIdx(APP_COMMON_MAPPING_128K),
																									HEAPID_BOX_APP );

  appwk->palRes[BOX2MAIN_PALRES_POKEMARK_SUB] = GFL_CLGRP_PLTT_Register(
																									ah,APP_COMMON_GetPokeMarkPltArcIdx(),
																									CLSYS_DRAW_SUB, PALNUM_MARK_S*0x20, HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_POKERUSICON] = GFL_CLGRP_CGR_Register(
																								ah,
																								APP_COMMON_GetPokerusCharArcIdx(APP_COMMON_MAPPING_128K),
																								FALSE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );

	appwk->celRes[BOX2MAIN_CELRES_POKERUSICON] = GFL_CLGRP_CELLANIM_Register(
																								ah,
																								APP_COMMON_GetPokerusCellArcIdx(APP_COMMON_MAPPING_128K),
																								APP_COMMON_GetPokerusAnimeArcIdx(APP_COMMON_MAPPING_128K),
																								HEAPID_BOX_APP );

  appwk->palRes[BOX2MAIN_PALRES_POKERUSICON] = GFL_CLGRP_PLTT_Register(
																								ah, APP_COMMON_GetPokerusPltArcIdx(),
																								CLSYS_DRAW_SUB, PALNUM_POKERUS_S*0x20, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * �^�C�v�A�C�R���ǉ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TypeIconObjAdd( BOX2_APP_WORK * appwk )
{
	BOX_CLWK_DATA	dat;
	u32	i;

	dat = ClaTypeIconParam;

	for( i=0; i<POKETYPE_MAX; i++ ){
		dat.chrRes = BOX2MAIN_CHRRES_TYPEICON + i;
		appwk->clwk[BOX2OBJ_ID_TYPEICON+i] = ClactWorkCreate( appwk, &dat );
		GFL_CLACT_WK_SetPlttOffs(
			appwk->clwk[BOX2OBJ_ID_TYPEICON+i],
			APP_COMMON_GetPokeTypePltOffset(i), CLWK_PLTTOFFS_MODE_PLTT_TOP );
		BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_TYPEICON+i, FALSE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �^�C�v�A�C�R���؂�ւ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	type	�^�C�v
 * @param	objID	OBJ ID
 * @param	resID	���\�[�X�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void TypeIconChange( BOX2_APP_WORK * appwk, u32 type, u32 objID, u32 resID )
{
	NNSG2dCharacterData * chr;
	void * buf;

	buf = GFL_ARC_UTIL_LoadOBJCharacter(
					APP_COMMON_GetArcId(),
					APP_COMMON_GetPokeTypeCharArcIdx(type), FALSE, &chr, HEAPID_BOX_APP );
	GFL_CLGRP_CGR_Replace( appwk->chrRes[resID], chr );
	GFL_HEAP_FreeMemory( buf );

	GFL_CLACT_WK_SetPlttOffs(
		appwk->clwk[objID], APP_COMMON_GetPokeTypePltOffset(type), CLWK_PLTTOFFS_MODE_PLTT_TOP );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �^�C�v�A�C�R���؂�ւ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	info	�\���f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
#define	TYPEICON1_PX		( 88 )
#define	TYPEICON2_PX		( 122 )
#define	TYPEICON_PY			( 56 )

static void TypeIconPut( BOX2_APP_WORK * appwk, u32 type1, u32 type2 )
{
	u32	objID;

	objID = BOX2OBJ_ID_TYPEICON + type1;
	BOX2OBJ_Vanish( appwk, objID, TRUE );
	BOX2OBJ_SetPos( appwk, objID, TYPEICON1_PX, TYPEICON_PY, CLSYS_DEFREND_SUB );

	if( type2 != 0 && type1 != type2 ){
		objID = BOX2OBJ_ID_TYPEICON + type2;
		BOX2OBJ_Vanish( appwk, objID, TRUE );
		BOX2OBJ_SetPos( appwk, objID, TYPEICON2_PX, TYPEICON_PY, CLSYS_DEFREND_SUB );
	}
}

void BOX2OBJ_TypeIconChange( BOX2_APP_WORK * appwk, BOX2_POKEINFO_DATA * info )
{
	u32	i;

	// �S�^�C�v�A�C�R���I�t
	for( i=BOX2OBJ_ID_TYPEICON; i<BOX2OBJ_ID_TYPEICON+POKETYPE_MAX; i++ ){
		BOX2OBJ_Vanish( appwk, i, FALSE );
	}

	// �^�}�S
	if( info->tamago != 0 ){ return; }

	TypeIconPut( appwk, info->type1, info->type2 );
}


//============================================================================================
//	�ǎ��A�C�R��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��A�C�R�����\�[�X�ǂݍ���
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void WallPaperIconResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
	u32	i;
	
	ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	// �L����
	for( i=0; i<BOX2OBJ_WPICON_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_WPICON+i] = GFL_CLGRP_CGR_Register(
																								ah, NARC_box_gra_box_wp_image_NCGR,
																								FALSE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	}
	// �Z���E�A�j��
	appwk->celRes[BOX2MAIN_CELRES_WPICON] = GFL_CLGRP_CELLANIM_Register(
																						ah,
																						NARC_box_gra_box_wp_image_NCER,
																						NARC_box_gra_box_wp_image_NANR,
																						HEAPID_BOX_APP );
	// �p���b�g
  appwk->palRes[BOX2MAIN_PALRES_WPICON] = GFL_CLGRP_PLTT_Register(
																						ah, NARC_box_gra_box_wp_image_NCLR,
																						CLSYS_DRAW_MAIN, PALNUM_WPICON*0x20, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��A�C�R��OBJ�ǉ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void WallPaperObjAdd( BOX2_APP_WORK * appwk )
{
	BOX_CLWK_DATA	prm;
	u32	i;

	for( i=0; i<BOX2OBJ_WPICON_MAX; i++ ){
		prm = ClaWallPaperIconParam;
		prm.dat.pos_x += BOX2OBJ_WPICON_SX * i;
		prm.chrRes = BOX2MAIN_CHRRES_WPICON + i;
		appwk->clwk[BOX2OBJ_ID_WPICON+i] = ClactWorkCreate( appwk, &prm );
	}
}
*/


//============================================================================================
//	�A�C�e���A�C�R��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R�����\�[�X�ǂݍ���
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemIconDummyResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
/*
	ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_ITEMICON] = GFL_CLGRP_CGR_Register(
																							ah, NARC_box_gra_box_itemicon_lz_NCGR,
																							TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_ITEMICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_box_itemicon_NCER,
																							NARC_box_gra_box_itemicon_NANR,
																							HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_ITEMICON_SUB] = GFL_CLGRP_CGR_Register(
																									ah, NARC_box_gra_box_itemicon_lz_NCGR,
																									TRUE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
*/
  ah = GFL_ARC_OpenDataHandle( ARCID_ITEMICON, HEAPID_BOX_APP );

  appwk->palRes[BOX2MAIN_PALRES_ITEMICON] = GFL_CLGRP_PLTT_Register(
																							ah, ITEM_GetIndex(0,ITEM_GET_ICON_PAL),
																							CLSYS_DRAW_MAIN, PALNUM_ITEMICON*0x20, HEAPID_BOX_APP );
  appwk->palRes[BOX2MAIN_PALRES_ITEMICON_SUB] = GFL_CLGRP_PLTT_Register(
																									ah, ITEM_GetIndex(0,ITEM_GET_ICON_PAL),
																									CLSYS_DRAW_SUB, PALNUM_ITEMICON_S*0x20, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���O���t�B�b�N�؂�ւ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	item	�A�C�e���ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconChange( BOX2_APP_WORK * appwk, u16 item )
{
	NNSG2dCharacterData * chr;
	NNSG2dPaletteData * pal;
	void * buf;

	buf = GFL_ARC_UTIL_LoadOBJCharacter(
					ITEM_GetIconArcID(), ITEM_GetIndex(item,ITEM_GET_ICON_CGX), 0, &chr, HEAPID_BOX_APP );
	GFL_CLGRP_CGR_Replace( GFL_CLACT_WK_GetCgrIndex(appwk->clwk[BOX2OBJ_ID_ITEMICON]), chr );
	GFL_HEAP_FreeMemory( buf );

	buf = GFL_ARC_UTIL_LoadPalette(
					ITEM_GetIconArcID(), ITEM_GetIndex(item,ITEM_GET_ICON_PAL), &pal, HEAPID_BOX_APP );
	GFL_CLGRP_PLTT_Replace( GFL_CLACT_WK_GetPlttIndex(appwk->clwk[BOX2OBJ_ID_ITEMICON]), pal, 1 );
	GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���O���t�B�b�N�؂�ւ��i�T�u��ʗp�j
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	item	�A�C�e���ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconChangeSub( BOX2_APP_WORK * appwk, u16 item )
{
	NNSG2dCharacterData * chr;
	NNSG2dPaletteData * pal;
	void * buf;

	buf = GFL_ARC_UTIL_LoadOBJCharacter(
					ITEM_GetIconArcID(), ITEM_GetIndex(item,ITEM_GET_ICON_CGX), 0, &chr, HEAPID_BOX_APP );
	GFL_CLGRP_CGR_Replace( GFL_CLACT_WK_GetCgrIndex(appwk->clwk[BOX2OBJ_ID_ITEMICON_SUB]), chr );
	GFL_HEAP_FreeMemory( buf );

	buf = GFL_ARC_UTIL_LoadPalette(
					ITEM_GetIconArcID(), ITEM_GetIndex(item,ITEM_GET_ICON_PAL), &pal, HEAPID_BOX_APP );
	GFL_CLGRP_PLTT_Replace( GFL_CLACT_WK_GetPlttIndex(appwk->clwk[BOX2OBJ_ID_ITEMICON_SUB]), pal, 1 );
	GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���g�k�ݒ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	flg		TRUE = �g�k, FALSE = �ʏ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconAffineSet( BOX2_APP_WORK * appwk, BOOL flg )
{
	GFL_CLACTPOS	pos;

	if( flg == TRUE ){
		GFL_CLACT_WK_SetAffineParam( appwk->clwk[BOX2OBJ_ID_ITEMICON], CLSYS_AFFINETYPE_NORMAL );
		pos.x = 12;
		pos.y = 12;
		GFL_CLACT_WK_SetAffinePos( appwk->clwk[BOX2OBJ_ID_ITEMICON], &pos );
	}else{
		pos.x = 0;
		pos.y = 0;
		GFL_CLACT_WK_SetAffinePos( appwk->clwk[BOX2OBJ_ID_ITEMICON], &pos );
		GFL_CLACT_WK_SetAffineParam( appwk->clwk[BOX2OBJ_ID_ITEMICON], CLSYS_AFFINETYPE_NONE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R�����W�ݒ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	x		�w���W
 * @param	y		�x���W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconPosSet( BOX2_APP_WORK * appwk, s16 x, s16 y )
{
	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_ITEMICON, x, y, CLSYS_DEFREND_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���ړ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	mx		�w���W�ړ���
 * @param	my		�x���W�ړ���
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconMove( BOX2_APP_WORK * appwk, s16 mx, s16 my )
{
	s16	px, py;

	BOX2OBJ_GetPos( appwk, BOX2OBJ_ID_ITEMICON, &px, &py, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_ITEMICON, px+mx, py+my, CLSYS_DEFREND_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * �w��|�P�����A�C�R���̈ʒu�ɃA�C�e���A�C�R�����Z�b�g
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	poke	�|�P�����A�C�R���ʒu
 * @param	mode	�|�P�����A�C�R���\�����[�h
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconPokePut( BOX2_APP_WORK * appwk, u32 poke, u32 mode )
{
	s16	x, y;

	BOX2OBJ_PokeIconDefaultPosGet( poke, &x, &y, mode );
	BOX2OBJ_ItemIconPosSet( appwk, x+8, y+8 );
}

//--------------------------------------------------------------------------------------------
/**
 * �w��|�P�����A�C�R���̈ʒu�ɃA�C�e���A�C�R���̒͂񂾈ʒu���Z�b�g
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	poke	�|�P�����A�C�R���ʒu
 * @param	mode	�|�P�����A�C�R���\�����[�h
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconPokePutHand( BOX2_APP_WORK * appwk, u32 poke, u32 mode )
{
	s16	x, y;

	BOX2OBJ_PokeIconDefaultPosGet( poke, &x, &y, mode );
	BOX2OBJ_ItemIconPosSet( appwk, x, y+4 );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���̃A�E�g���C���J�[�\����ǉ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconCursorAdd( BOX2_APP_WORK * appwk )
{
	s16	x, y;
	u32	i;

	if( appwk->get_item == ITEM_DUMMY_DATA ){
		return;
	}

	BOX2OBJ_GetPos( appwk, BOX2OBJ_ID_ITEMICON, &x, &y, CLSYS_DEFREND_MAIN );

	// �V�K�ǉ�
	if( appwk->clwk[BOX2OBJ_ID_OUTLINE] == NULL ){
		BOX_CLWK_DATA	prm = ClactParamTbl[BOX2OBJ_ID_ITEMICON];
		prm.dat.softpri = GFL_CLACT_WK_GetSoftPri( appwk->clwk[BOX2OBJ_ID_ITEMICON] ) + 1;
		prm.dat.bgpri   = GFL_CLACT_WK_GetBgPri( appwk->clwk[BOX2OBJ_ID_ITEMICON] );
		prm.chrRes      = BOX2MAIN_CHRRES_ITEMICON;
		prm.palRes      = BOX2MAIN_PALRES_BOXOBJ;
		prm.pal         = PALNUM_OUTLINE;

//		appwk->obj_trans_stop = 1;
		for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
			prm.dat.pos_x = x + PokeCursorXTbl[i];
			prm.dat.pos_y = y + PokeCursorYTbl[i];

			appwk->clwk[BOX2OBJ_ID_OUTLINE+i] = ClactWorkCreate( appwk, &prm );
//			GFL_CLACT_WK_SetPlttOffs(
//				appwk->clwk[BOX2OBJ_ID_OUTLINE+i], PALNUM_OUTLINE, CLWK_PLTTOFFS_MODE_OAM_COLOR );
			GFL_CLACT_WK_SetPlttOffs( appwk->clwk[BOX2OBJ_ID_OUTLINE+i], prm.pal, CLWK_PLTTOFFS_MODE_PLTT_TOP );
			BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_OUTLINE+i, FALSE );
		}
//		appwk->obj_trans_stop = 0;
	}else{
		int	obj_pri, bg_pri;

		obj_pri = GFL_CLACT_WK_GetSoftPri( appwk->clwk[BOX2OBJ_ID_ITEMICON] ) + 1;
		bg_pri  = GFL_CLACT_WK_GetBgPri( appwk->clwk[BOX2OBJ_ID_ITEMICON] );

		for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
			BOX2OBJ_SetPos(
				appwk, BOX2OBJ_ID_OUTLINE+i,
				x+PokeCursorXTbl[i], y+PokeCursorYTbl[i], CLSYS_DEFREND_MAIN );
			BOX2OBJ_ObjPriChange( appwk, BOX2OBJ_ID_OUTLINE+i, obj_pri );
			BOX2OBJ_BgPriChange( appwk, BOX2OBJ_ID_OUTLINE+i, bg_pri );
			BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_OUTLINE+i, FALSE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���̃A�E�g���C���J�[�\����\��
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconCursorOn( BOX2_APP_WORK * appwk )
{
	u32	i;

	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_OUTLINE+i, TRUE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���̃A�E�g���C���J�[�\���ړ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconCursorMove( BOX2_APP_WORK * appwk )
{
	u32	i;
	s16	x, y;

	if( appwk->clwk[BOX2OBJ_ID_OUTLINE] == NULL ){ return; }

	BOX2OBJ_GetPos( appwk, BOX2OBJ_ID_ITEMICON, &x, &y, CLSYS_DEFREND_MAIN );

	// �A�E�g���C��
	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		BOX2OBJ_SetPos(
			appwk, BOX2OBJ_ID_OUTLINE+i,
			x + PokeCursorXTbl[i],
			y + PokeCursorYTbl[i],
			CLSYS_DEFREND_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���z�u�i�T�u��ʗp�j
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconPutSub( BOX2_APP_WORK * appwk )
{
/*
	s16	objy;
	s8	wfmx, wfmy;

	BGWINFRM_PosGet( appwk->wfrm, BOX2MAIN_WINFRM_SUBDISP, &wfmx, &wfmy );
	objy = (s16)wfmy * 8 + BOX_SUB_ACTOR_DEF_PY + SUBDISP_ITEMICON_PY;
	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_ITEMICON_SUB, SUBDISP_ITEMICON_PX, objy, CLSYS_DEFREND_SUB );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_ITEMICON_SUB, TRUE );
*/
}


//============================================================================================
//	���̑�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X��ʃ��\�[�X�ǂݍ���
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void BoxObjResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_BOXOBJ] = GFL_CLGRP_CGR_Register(
																						ah, NARC_box_gra_box_m_obj_lz_NCGR,
																						TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
  appwk->palRes[BOX2MAIN_PALRES_BOXOBJ] = GFL_CLGRP_PLTT_Register(
																						ah, NARC_box_gra_box_m_obj2_NCLR,
																						CLSYS_DRAW_MAIN, PALNUM_BOXOBJ*0x20, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_BOXOBJ] = GFL_CLGRP_CELLANIM_Register(
																						ah,
																						NARC_box_gra_box_m_obj_NCER,
																						NARC_box_gra_box_m_obj_NANR,
																						HEAPID_BOX_APP );
  GFL_ARC_CloseDataHandle( ah );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X���OBJ�ǉ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxObjAdd( BOX2_APP_WORK * appwk )
{
	u32	i;

	appwk->clwk[BOX2OBJ_ID_L_ARROW] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_L_ARROW] );
	appwk->clwk[BOX2OBJ_ID_R_ARROW] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_R_ARROW] );
//	appwk->clwk[BOX2OBJ_ID_BOXMV_LA] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_BOXMV_LA] );
//	appwk->clwk[BOX2OBJ_ID_BOXMV_RA] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_BOXMV_RA] );
	appwk->clwk[BOX2OBJ_ID_TRAY_CUR] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TRAY_CUR] );
	appwk->clwk[BOX2OBJ_ID_TRAY_NAME] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TRAY_NAME] );
//	appwk->clwk[BOX2OBJ_ID_TRAY_ARROW] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TRAY_ARROW] );
	appwk->clwk[BOX2OBJ_ID_HAND_CURSOR] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_HAND_CURSOR] );
	appwk->clwk[BOX2OBJ_ID_HAND_SHADOW] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_HAND_SHADOW] );
	appwk->clwk[BOX2OBJ_ID_ITEMICON] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_ITEMICON] );
	appwk->clwk[BOX2OBJ_ID_ITEMICON_SUB] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_ITEMICON_SUB] );
	appwk->clwk[BOX2OBJ_ID_TB_CANCEL] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TB_CANCEL] );
	appwk->clwk[BOX2OBJ_ID_TB_END] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TB_END] );
	appwk->clwk[BOX2OBJ_ID_TB_STATUS] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TB_STATUS] );

	for( i=0; i<6; i++ ){
		appwk->clwk[BOX2OBJ_ID_MARK1_S+i] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_MARK1_S+i] );
	}
	appwk->clwk[BOX2OBJ_ID_RARE] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_RARE] );
	appwk->clwk[BOX2OBJ_ID_POKERUS] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_POKERUS] );
	appwk->clwk[BOX2OBJ_ID_POKERUS_ICON] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_POKERUS_ICON] );

	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_HAND_SHADOW, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_ITEMICON, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_ITEMICON_SUB, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_RARE, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_POKERUS, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_POKERUS_ICON, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * ��J�[�\���\���ؑ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	flg		ON = �\���AOFF = ��\��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_HandCursorVanish( BOX2_APP_WORK * appwk, BOOL flg )
{
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_HAND_CURSOR, flg );
}

//--------------------------------------------------------------------------------------------
/**
 * ��J�[�\���ړ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	px		�w���W
 * @param	py		�x���W
 * @param	shadow	�e�\���t���O ON = �\���AOFF = ��\��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_HandCursorSet( BOX2_APP_WORK * appwk, s16 px, s16 py, BOOL shadow )
{
	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_HAND_CURSOR, px, py, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_HAND_SHADOW, px, py+24, CLSYS_DEFREND_MAIN );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_HAND_SHADOW, shadow );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�E�g���C���J�[�\���ǉ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeCursorAdd( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PokeCursorAdd2( syswk, syswk->get_pos );
}

void BOX2OBJ_PokeCursorAdd2( BOX2_SYS_WORK * syswk, u32 pos )
{
	BOX2_APP_WORK * appwk;
	s16	x, y;
	u16	posID;
	u16	i;

	if( pos == BOX2MAIN_GETPOS_NONE ){
		return;
	}

	appwk = syswk->app;
	posID = appwk->pokeicon_id[ pos ];
	BOX2OBJ_GetPos( appwk, posID, &x, &y, CLSYS_DEFREND_MAIN );

	// �V�K�ǉ�
	if( appwk->clwk[BOX2OBJ_ID_OUTLINE] == NULL ){
		BOX_CLWK_DATA	prm = ClaPokeIconParam;
		prm.dat.softpri = GFL_CLACT_WK_GetSoftPri( appwk->clwk[posID] ) + 1;
		prm.dat.bgpri   = GFL_CLACT_WK_GetBgPri( appwk->clwk[posID] );
		prm.chrRes      = BOX2MAIN_CHRRES_POKEICON + posID - BOX2OBJ_ID_POKEICON;
		prm.palRes      = BOX2MAIN_PALRES_BOXOBJ;
		prm.pal         = PALNUM_OUTLINE;

//		appwk->obj_trans_stop = 1;
		for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
			prm.dat.pos_x = x + PokeCursorXTbl[i];
			prm.dat.pos_y = y + PokeCursorYTbl[i];

			appwk->clwk[BOX2OBJ_ID_OUTLINE+i] = ClactWorkCreate( appwk, &prm );
			GFL_CLACT_WK_SetPlttOffs( appwk->clwk[BOX2OBJ_ID_OUTLINE+i], prm.pal, CLWK_PLTTOFFS_MODE_PLTT_TOP );
		}
//		appwk->obj_trans_stop = 0;
	// �L��������������
	}else{
		NNSG2dImageProxy	ipc;
		int	obj_pri, bg_pri;

		GFL_CLACT_WK_GetImgProxy( appwk->clwk[posID], &ipc );

		obj_pri = GFL_CLACT_WK_GetSoftPri( appwk->clwk[posID] ) + 1;
		bg_pri  = GFL_CLACT_WK_GetBgPri( appwk->clwk[posID] );

		for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
			GFL_CLACT_WK_SetImgProxy( appwk->clwk[BOX2OBJ_ID_OUTLINE+i], &ipc );
			BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_OUTLINE+i, x+PokeCursorXTbl[i], y+PokeCursorYTbl[i], CLSYS_DEFREND_MAIN );
			BOX2OBJ_ObjPriChange( appwk, BOX2OBJ_ID_OUTLINE+i, obj_pri );
			BOX2OBJ_BgPriChange( appwk, BOX2OBJ_ID_OUTLINE+i, bg_pri );
			BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_OUTLINE+i, TRUE );
		}
	}
}


//--------------------------------------------------------------------------------------------
/**
 * �A�E�g���C���J�[�\���\���؂�ւ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	flg		�\���t���O
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeCursorVanish( BOX2_SYS_WORK * syswk, BOOL flg )
{
	u32	i;

	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		if( syswk->app->clwk[BOX2OBJ_ID_OUTLINE+i] != NULL ){
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_OUTLINE+i, flg );
			if( flg == TRUE && syswk->get_pos != BOX2MAIN_GETPOS_NONE ){
				int	pri;
				u32	posID;
				posID = syswk->app->pokeicon_id[ syswk->get_pos ];

				pri = GFL_CLACT_WK_GetSoftPri( syswk->app->clwk[posID] );
				BOX2OBJ_ObjPriChange( syswk->app, BOX2OBJ_ID_OUTLINE+i, pri+1 );

				pri = GFL_CLACT_WK_GetBgPri( syswk->app->clwk[posID] );
				BOX2OBJ_BgPriChange( syswk->app, BOX2OBJ_ID_OUTLINE+i, pri );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �A�E�g���C���J�[�\���ړ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeCursorMove( BOX2_APP_WORK * appwk, u32 pos )
{
/*
	BOX2_APP_WORK * appwk;
	s16	x, y;
	u16	posID;
	u16	i;

	if( syswk->get_pos == BOX2MAIN_GETPOS_NONE ){
		return;
	}

	appwk = syswk->app;
	posID = appwk->pokeicon_id[ syswk->get_pos ];
	BOX2OBJ_GetPos( appwk, posID, &x, &y, CLSYS_DEFREND_MAIN );

	// �A�E�g���C��
	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		BOX2OBJ_SetPos(
			appwk, BOX2OBJ_ID_OUTLINE+i,
			x+PokeCursorXTbl[i], y+PokeCursorYTbl[i], CLSYS_DEFREND_MAIN );
	}
*/
	u32	i;
	s16	x, y;

	if( pos == BOX2MAIN_GETPOS_NONE ){
		return;
	}

	BOX2OBJ_GetPos( appwk, appwk->pokeicon_id[pos], &x, &y, CLSYS_DEFREND_MAIN );

	// �A�E�g���C��
	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		BOX2OBJ_SetPos(
			appwk, BOX2OBJ_ID_OUTLINE+i,
			x+PokeCursorXTbl[i], y+PokeCursorYTbl[i], CLSYS_DEFREND_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�A�C�R�����\�[�X�ǂݍ���
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void TrayIconResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
	u32	i;

	ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_TRAYICON+i] = GFL_CLGRP_CGR_Register(
																									ah, NARC_box_gra_box_tray_lz_NCGR,
																									TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	}
  appwk->palRes[BOX2MAIN_PALRES_TRAYICON] = GFL_CLGRP_PLTT_Register(
																							ah, NARC_box_gra_box_tray_NCLR,
																							CLSYS_DRAW_MAIN, PALNUM_TRAYICON*0x20, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_TRAYICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_box_tray_NCER,
																							NARC_box_gra_box_tray_NANR,
																							HEAPID_BOX_APP );
  GFL_ARC_CloseDataHandle( ah );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �g���C�A�C�R��OBJ�ǉ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TrayObjAdd( BOX2_APP_WORK * appwk )
{
	BOX_CLWK_DATA	prm;
	u32	i;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		prm = ClaTrayIconParam;
		prm.dat.pos_y += BOX2OBJ_TRAYICON_SY * i;
		prm.chrRes = BOX2MAIN_CHRRES_TRAYICON + i;
		appwk->clwk[BOX2OBJ_ID_TRAYICON+i] = ClactWorkCreate( appwk, &prm );
	}
/*
	TCATS_OBJECT_ADD_PARAM_S	prm;
	u32	i;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		prm = ClaTrayIconParam;
		prm.x += BOX2OBJ_TRAYICON_SX * i;
		prm.id[0] = CHR_ID_TRAYICON + i;
		appwk->cap[BOX2OBJ_ID_TRAYICON+i] = CATS_ObjectAdd_S( appwk->csp, appwk->crp, &prm );
	}
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�؂�ւ����\���ؑ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	flg		ON/OFF�t���O
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayMoveArrowVanish( BOX2_APP_WORK * appwk, BOOL flg )
{
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_L_ARROW, flg );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_R_ARROW, flg );
}


//============================================================================================
//	�{�b�N�X�ړ��t���[���֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�ړ��F�g���C���Ǝ��[���̂n�`�l�t�H���g���ړ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void BoxMoveFntOamPos( BOX2_APP_WORK * appwk )
{
	FONTOAM_SetMat( appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, -72, -8 );
	FONTOAM_SetMat( appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NUM].oam, 32, -8 );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�ړ��F�e�n�a�i��������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BoxMoveObjInit( BOX2_SYS_WORK * syswk )
{
	BOX2_APP_WORK * appwk = syswk->app;

//	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_BOXMV_LA, BOXMV_LA_PX, BOXMV_LA_PY, CLSYS_DEFREND_MAIN );
//	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_BOXMV_RA, BOXMV_RA_PX, BOXMV_RA_PY, CLSYS_DEFREND_MAIN );

/*
	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_TRAY_CUR, BOXMV_TRAY_CUR_PX, BOXMV_TRAY_CUR_PY, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_TRAY_NAME, BOXMV_TRAY_NAME_PX, BOXMV_TRAY_NAME_PY, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_TRAY_ARROW, BOXMV_TRAY_ARROW_PX, BOXMV_TRAY_ARROW_PY, CLSYS_DEFREND_MAIN );
*/

	BOX2OBJ_TrayIconCgxMake( syswk, syswk->tray );
	BOX2OBJ_InitTrayIconScroll( syswk );

//	BOX2OBJ_TrayIconChange( syswk );

//	BOX2OBJ_AnmSet( appwk, BOX2OBJ_ID_TRAY_NAME, BOX2OBJ_ANM_TRAY_NAME );

//	BoxMoveFntOamPos( appwk );
/*
	FONTOAM_SetDrawFlag( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, TRUE );
	FONTOAM_SetDrawFlag( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM].oam, TRUE );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�ړ��F�e�n�a�i���X�N���[��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	mv		�ړ���
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BoxMoveFrmScroll( BOX2_SYS_WORK * syswk, s16 mv )
{
	u32	i;
	s16	x, y;

/*
	for( i=BOX2OBJ_ID_TRAY_CUR; i<=BOX2OBJ_ID_TRAY_ARROW; i++ ){
		BOX2OBJ_GetPos( syswk->app, i, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos( syswk->app, i, x, y+mv, CLSYS_DEFREND_MAIN );
	}
*/
	BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, &x, &y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, x-mv, y, CLSYS_DEFREND_MAIN );

//	BoxMoveFntOamPos( syswk->app );

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, x-mv, y, CLSYS_DEFREND_MAIN );

		BmpOam_ActorGetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, &x, &y );
		BmpOam_ActorSetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, x-mv, y );
	}
}

// �J�[�\���������ʒu��
void BOX2OBJ_InitBoxMoveCursorPos( BOX2_APP_WORK * appwk )
{
	BOX2OBJ_SetPos(
		appwk, BOX2OBJ_ID_TRAY_CUR, BOX2OBJ_TRAYICON_PX, BOX2OBJ_TRAYICON_SY, CLSYS_DEFREND_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�ړ��F�J�[�\���Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BoxMoveCursorSet( BOX2_SYS_WORK * syswk )
{
/*
	u32	pos;
	s16	x, y;

	if( (syswk->box_mv_pos/BOX2OBJ_TRAYICON_MAX) == (syswk->tray/BOX2OBJ_TRAYICON_MAX) ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, TRUE );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, FALSE );
	}

	pos = syswk->tray % BOX2OBJ_TRAYICON_MAX;

	BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, &x, &y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos(
		syswk->app, BOX2OBJ_ID_TRAY_CUR,
		BOX2OBJ_TRAYICON_PX+BOX2OBJ_TRAYICON_SX*pos, y, CLSYS_DEFREND_MAIN );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �{�b�N�X�ړ��F�g���C���֘A�n�a�i�Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BoxMoveNameSet( BOX2_SYS_WORK * syswk )
{
/*
	s16	x, y, px;
	u16	pos;

	pos = syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;

	BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_NAME, &x, &y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos(
		syswk->app, BOX2OBJ_ID_TRAY_NAME, BoxNamePosTbl[pos], y, CLSYS_DEFREND_MAIN );

	BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAYICON+pos, &px, &y, CLSYS_DEFREND_MAIN );
//	BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_ARROW, &x, &y, CLSYS_DEFREND_MAIN );
//	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_ARROW, px, y, CLSYS_DEFREND_MAIN );

//	BoxMoveFntOamPos( syswk->app );
*/
}

// �g���C�A�C�R�����������W�ֈړ�
void BOX2OBJ_InitTrayIconScroll( BOX2_SYS_WORK * syswk )
{
	BOX_CLWK_DATA	prm;
	s16	i;
	u16	tray;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		tray = BOX2MAIN_GetBoxMoveTrayNum( syswk, i-1 );

		prm = ClaTrayIconParam;
		prm.dat.pos_y += BOX2OBJ_TRAYICON_SY * i;
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, prm.dat.pos_x, prm.dat.pos_y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_TrayIconCgxTransIdx( syswk, tray, BOX2OBJ_ID_TRAYICON+i );

		BmpOam_ActorSetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, BOX2OBJ_TRAYNUM_PX, BOX2OBJ_TRAYNUM_PY+BOX2OBJ_TRAYNUM_SY*i );
		BOX2BMP_WriteTrayNum( syswk, tray, BOX2MAIN_FNTOAM_TRAY_NUM1+i );
//		BmpOam_ActorSetDrawEnable( syswk->app->fobj[i].oam, TRUE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�A�C�R���̍��W���擾
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	pos		�g���C�\���ʒu
 * @param	x		�w���W�i�[�ꏊ
 * @param	y		�x���W�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconPosGet( BOX2_APP_WORK * appwk, u32 pos, s16 * x, s16 * y )
{
	u32	i;

	pos = ( pos + 1 ) * BOX2OBJ_TRAYICON_SY;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		BOX2OBJ_GetPos( appwk, BOX2OBJ_ID_TRAYICON+i, x, y, CLSYS_DEFREND_MAIN );
		if( pos == *y ){
			return;
		}
	}

	*x = 0;
	*y = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�A�C�R���̃L�����f�[�^�쐬�i�ʁj
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		tray		�g���C�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconCgxMake( BOX2_SYS_WORK * syswk, u32 tray )
{
	NNSG2dCharacterData * chr;
	void * buf;

	buf = GFL_ARC_UTIL_LoadOBJCharacter(
					ARCID_BOX2_GRA, NARC_box_gra_box_tray_lz_NCGR, TRUE, &chr, HEAPID_BOX_APP );
	GFL_STD_MemCopy( chr->pRawData, syswk->app->trayicon_cgx[tray], TRAY_ICON_CGX_SIZ );
	GFL_HEAP_FreeMemory( buf );

	WallPaperBufferFill(
		syswk,
		syswk->app->trayicon_cgx[tray],
		BOX2MAIN_GetWallPaperNumber( syswk, tray ),
		TRAY_ICON_CHG_COL,
		TRAY_ICON_CGX_SIZ );

	TrayPokeDotPut( syswk, tray, syswk->app->trayicon_cgx[tray] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�A�C�R���̃L�����f�[�^�쐬�i�S���j
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconCgxMakeAll( BOX2_SYS_WORK * syswk )
{
	u32	i;

	for( i=0; i<BOX_MAX_TRAY; i++ ){
		BOX2OBJ_TrayIconCgxMake( syswk, i );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���C�A�C�R���̃L�����f�[�^�؂�ւ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconChange( BOX2_SYS_WORK * syswk )
{
	u32	id;
	s32	pos;
	u32	i;

//	pos = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos);
	pos = syswk->box_mv_pos - 1;
	if( pos < 0 ){
		pos = syswk->trayMax - 1;
	}
	id  = BOX2OBJ_ID_TRAYICON;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		GFL_CLGRP_CGR_ReplaceEx(
			GFL_CLACT_WK_GetCgrIndex( syswk->app->clwk[id] ),
			syswk->app->trayicon_cgx[pos], TRAY_ICON_CGX_SIZ, 0, CLSYS_DRAW_MAIN );
		id++;
		pos++;
		if( pos >= syswk->trayMax ){
			pos = 0;
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �g���C�A�C�R���̃L������]��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	tray	�g���C�ԍ�
 *
 * @return	none
 *
 * @li	�w��g���C�ƃA�C�R������v���Ȃ��ꍇ�͓]�����Ȃ�
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconCgxTrans( BOX2_SYS_WORK * syswk, u32 tray )
{
/*
	if( (syswk->box_mv_pos/BOX2OBJ_TRAYICON_MAX) != (tray/BOX2OBJ_TRAYICON_MAX) ){
		return;
	}
*/

/*
	s32	pos;
	u32	i;

	pos = syswk->box_mv_pos - 1;
	if( pos < 0 ){
		pos = syswk->trayMax - 1;
	}
	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		if( pos == tray ){ break; }
		pos++;
		if( pos >= syswk->trayMax ){
			pos = 0;
		}
	}
	if( i == BOX2OBJ_TRAYICON_MAX ){
		return;
	}

	GFL_CLGRP_CGR_ReplaceEx(
//		GFL_CLACT_WK_GetCgrIndex( syswk->app->clwk[BOX2OBJ_ID_TRAYICON+i+syswk->app->box_mv_scroll] ),
		GFL_CLACT_WK_GetCgrIndex( syswk->app->clwk[BOX2OBJ_ID_TRAYICON+i] ),
		syswk->app->trayicon_cgx[tray], TRAY_ICON_CGX_SIZ, 0, CLSYS_DRAW_MAIN );
*/
	s16	pos;
	s16	i;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		pos = BOX2MAIN_GetBoxMoveTrayNum( syswk, i-1 );
		if( pos == tray ){
			BOX2OBJ_TrayIconCgxTransPos( syswk, tray, i );
			break;
		}
	}
}

void BOX2OBJ_TrayIconCgxTransIdx( BOX2_SYS_WORK * syswk, u32 tray, u32 idx )
{
	GFL_CLGRP_CGR_ReplaceEx(
		GFL_CLACT_WK_GetCgrIndex( syswk->app->clwk[idx] ),
		syswk->app->trayicon_cgx[tray], TRAY_ICON_CGX_SIZ, 0, CLSYS_DRAW_MAIN );
}

void BOX2OBJ_TrayIconCgxTransPos( BOX2_SYS_WORK * syswk, u32 tray, u32 pos )
{
	u32	i;
	s16	x, y;

	pos *= BOX2OBJ_TRAYICON_SY;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, &x, &y, CLSYS_DEFREND_MAIN );
		if( pos == y ){
			BOX2OBJ_TrayIconCgxTransIdx( syswk, tray, BOX2OBJ_ID_TRAYICON+i );
			BOX2BMP_WriteTrayNum( syswk, tray, BOX2MAIN_FNTOAM_TRAY_NUM1+i );
			break;
		}
	}
}



//--------------------------------------------------------------------------------------------
/**
 * �|�P�����h�b�g��`��
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	tray	�g���C�ԍ�
 * @param	buf		�`���
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TrayPokeDotPut( BOX2_SYS_WORK * syswk, u32 tray, u8 * buf )
{
	POKEMON_PASO_PARAM * ppp;
	POKEMON_PERSONAL_DATA * ppd;
	BOOL fast;
	u32	mons;
	u16	color;
	u8	i, j, y;
	u8	px, py;

	py = TRAY_POKEDOT_PY;

	for( i=0; i<BOX2OBJ_POKEICON_V_MAX; i++ ){
		px = TRAY_POKEDOT_PX;
		for( j=0; j<BOX2OBJ_POKEICON_H_MAX; j++ ){
			ppp  = BOXDAT_GetPokeDataAddress( syswk->dat->sv_box, tray, i*BOX2OBJ_POKEICON_H_MAX+j );
			fast = PPP_FastModeOn( ppp );
			mons = PPP_Get( ppp, ID_PARA_monsno, NULL );
			if( PPP_Get( ppp, ID_PARA_poke_exist, NULL ) != 0 ){
				if( PPP_Get( ppp, ID_PARA_tamago_flag, NULL ) == 0 ){
					u16 frm = PPP_Get( ppp, ID_PARA_form_no, NULL );
					ppd = POKE_PERSONAL_OpenHandle( mons, frm, HEAPID_BOX_APP );
					color = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_color );
					POKE_PERSONAL_CloseHandle( ppd );
//					color = PokePersonalParaGetHandle( syswk->app->ppd_ah, mons, frm, ID_PER_color );
				}else{
					if( mons == MONSNO_MANAFI ){
						color = POKEPER_COLOR_BLUE;
					}else{
						color = POKEPER_COLOR_WHITE;
					}
				}
				color = ( PLANUM_TRAYPOKE << 4 ) + TrayPokeDotColorPos[color];
				color = ( color << 8 ) | color;
				for( y=py; y<py+TRAY_POKEDOT_SY; y++ ){
					GFL_STD_MemFill16( &buf[ ((((y>>3)<<2)+(px>>3))<<6) + ((y&7)<<3)+(px&7) ], color, 2 );
				}
			}
			PPP_FastModeOff( ppp, fast );
			px += TRAY_POKEDOT_SX;
		}
		py += TRAY_POKEDOT_SY;
	}
}


void BOX2OBJ_TrayIconScroll( BOX2_SYS_WORK * syswk, s16 mv )
{
	u32	i;
	s16	x, y, mvy;
	s16	nx, ny, nmvy;
	s16	prm;
	u16	tray;

	if( GFL_STD_Abs(mv) == BOX2MAIN_TRAYICON_SCROLL_CNT ){
		prm = 2;
//		prm = 4;
	}else{
//		prm = 4;
		prm = 8;
	}
	if( mv < 0 ){
		prm *= -1;
	}

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		// �g���C�A�C�R��
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, &x, &y, CLSYS_DEFREND_MAIN );
		mvy = y + prm;
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, x, mvy, CLSYS_DEFREND_MAIN );
		// �i�[��
		BmpOam_ActorGetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, &nx, &ny );
		nmvy = ny + prm;
		BmpOam_ActorSetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, nx, nmvy );

		if( mvy < -16 ){
			tray = BOX2MAIN_GetBoxMoveTrayNum( syswk, 4 );
			BOX2OBJ_TrayIconCgxTransIdx( syswk, tray, BOX2OBJ_ID_TRAYICON+i );
			BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, x, mvy+BOX2OBJ_TRAYICON_SY*BOX2OBJ_TRAYICON_MAX, CLSYS_DEFREND_MAIN );
			BOX2BMP_WriteTrayNum( syswk, tray, BOX2MAIN_FNTOAM_TRAY_NUM1+i );
			BmpOam_ActorSetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, nx, nmvy+BOX2OBJ_TRAYICON_SY*BOX2OBJ_TRAYICON_MAX );
		}else if( mvy >= (BOX2OBJ_TRAYICON_PY+BOX2OBJ_TRAYICON_SY*(BOX2OBJ_TRAYICON_MAX-1)+16) ){
			tray = BOX2MAIN_GetBoxMoveTrayNum( syswk, -1 );
			BOX2OBJ_TrayIconCgxTransIdx( syswk, tray, BOX2OBJ_ID_TRAYICON+i );
			BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, x, mvy-BOX2OBJ_TRAYICON_SY*BOX2OBJ_TRAYICON_MAX, CLSYS_DEFREND_MAIN );
			BOX2BMP_WriteTrayNum( syswk, tray, BOX2MAIN_FNTOAM_TRAY_NUM1+i );
			BmpOam_ActorSetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, nx, nmvy-BOX2OBJ_TRAYICON_SY*BOX2OBJ_TRAYICON_MAX );
		}
	}

	if( BOX2OBJ_VanishCheck( syswk->app, BOX2OBJ_ID_TRAY_CUR ) == TRUE ){
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, x, y+prm, CLSYS_DEFREND_MAIN );
	}
}

void BOX2OBJ_InitTrayCursorScroll( BOX2_SYS_WORK * syswk, s32 mv )
{
	s16	x, y;

	if( mv < 0 && BOX2MAIN_GetBoxMoveTrayNum( syswk, -1 ) == syswk->tray ){
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, x, -BOX2OBJ_TRAYICON_SY, CLSYS_DEFREND_MAIN );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, TRUE );
		return;
	}
	if( mv > 0 && BOX2MAIN_GetBoxMoveTrayNum( syswk, 4 ) == syswk->tray ){
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, x, BOX2OBJ_TRAYICON_SY*BOX2OBJ_TRAYICON_MAX, CLSYS_DEFREND_MAIN );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, TRUE );
		return;
	}
}

void BOX2OBJ_EndTrayCursorScroll( BOX2_SYS_WORK * syswk )
{
	u32	i;
	s16	x, y;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		if( BOX2MAIN_GetBoxMoveTrayNum( syswk, i-1 ) == syswk->tray ){
			BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, &x, &y, CLSYS_DEFREND_MAIN );
			BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, x, i*BOX2OBJ_TRAYICON_SY, CLSYS_DEFREND_MAIN );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, TRUE );
			return;
		}
	}
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, FALSE );
}

static const s16 TrayNamePosYTbl[] = {
	BOXNAME_OBJ_PY + BOX2OBJ_TRAYICON_SY,
	BOXNAME_OBJ_PY + BOX2OBJ_TRAYICON_SY*2,
	BOXNAME_OBJ_PY + BOX2OBJ_TRAYICON_SY*3,
	BOXNAME_OBJ_PY + BOX2OBJ_TRAYICON_SY*4,
};

void BOX2OBJ_SetTrayNamePos( BOX2_SYS_WORK * syswk, u32 pos )
{
	BOX2OBJ_SetPos(
		syswk->app, BOX2OBJ_ID_TRAY_NAME, BOXNAME_OBJ_PX, TrayNamePosYTbl[pos], CLSYS_DEFREND_MAIN );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_NAME, TRUE );

	BmpOam_ActorSetPos(
		syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam,
		BOXNAME_OBJ_PX - BOX2OBJ_FNTOAM_BOXNAME_SX*8/2,
		TrayNamePosYTbl[pos] - 8 );
	BmpOam_ActorSetDrawEnable( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, TRUE );
}

void BOX2OBJ_ChangeTrayName( BOX2_SYS_WORK * syswk, u32 pos, BOOL flg )
{
	if( flg == TRUE ){
		BOX2BMP_BoxMoveNameWrite( syswk, pos );
		BOX2OBJ_SetTrayNamePos( syswk, pos );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_NAME, FALSE );
		BmpOam_ActorSetDrawEnable( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, FALSE );
	}
}



//============================================================================================
//	�ǎ��ύX�֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX�F�ǎ����̂n�`�l�t�H���g���ړ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WallPaperChgFntOamPos( BOX2_APP_WORK * appwk )
{
//	FONTOAM_SetMat( appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, -48, -8 );
}

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX�F�n�a�i������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_WallPaperChgObjInit( BOX2_SYS_WORK * syswk )
{
	BOX2_APP_WORK * appwk = syswk->app;

//	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_BOXMV_LA, BOXMV_LA_PX, BOXMV_LA_PY, CLSYS_DEFREND_MAIN );
//	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_BOXMV_RA, BOXMV_RA_PX, BOXMV_RA_PY, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_TRAY_CUR, BOXMV_TRAY_CUR_PX, BOXMV_TRAY_CUR_PY, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_TRAY_NAME, BOXMV_TRAY_NAME_PX, BOXMV_TRAY_NAME_PY, CLSYS_DEFREND_MAIN );
//	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_TRAY_ARROW, BOXMV_TRAY_ARROW_PX, BOXMV_TRAY_ARROW_PY, CLSYS_DEFREND_MAIN );

	BOX2OBJ_WallPaperObjChange( syswk );

//	BOX2OBJ_AnmSet( appwk, BOX2OBJ_ID_TRAY_NAME, BOX2OBJ_ANM_WP_NAME );

	WallPaperChgFntOamPos( appwk );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX�F�n�a�i�X�N���[��
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	mv		�ړ���
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_WallPaperChgFrmScroll( BOX2_APP_WORK * appwk, s16 mv )
{
	u32	i;
	s16	x, y;

	WallPaperChgFntOamPos( appwk );

	for( i=0; i<BOX2OBJ_WPICON_MAX; i++ ){
		BOX2OBJ_GetPos( appwk, BOX2OBJ_ID_WPICON+i, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_WPICON+i, x, y+mv, CLSYS_DEFREND_MAIN );
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX�F�J�[�\���Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_WallPaperCursorSet( BOX2_SYS_WORK * syswk )
{
	u32	wp;
	u32	pos;
	s16	x, y;

	wp = BOX2MAIN_GetWallPaperNumber( syswk, syswk->box_mv_pos );

	if( (syswk->app->wallpaper_pos/BOX2OBJ_WPICON_MAX) == (wp/BOX2OBJ_WPICON_MAX) ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, TRUE );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, FALSE );
	}

	pos = wp % BOX2OBJ_WPICON_MAX;

	BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, &x, &y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos(
		syswk->app, BOX2OBJ_ID_TRAY_CUR,
		BOX2OBJ_WPICON_PX+BOX2OBJ_WPICON_SX*pos, y, CLSYS_DEFREND_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX�F���O�Z�b�g
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_WallPaperNameSet( BOX2_SYS_WORK * syswk )
{
	s16	x, y, px;
	u16	pos;

	pos = syswk->app->wallpaper_pos % BOX2OBJ_WPICON_MAX;

	BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_WPICON+pos, &px, &y, CLSYS_DEFREND_MAIN );

	BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_NAME, &x, &y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_NAME, px, y, CLSYS_DEFREND_MAIN );

//	BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_ARROW, &x, &y, CLSYS_DEFREND_MAIN );
//	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_ARROW, px, y, CLSYS_DEFREND_MAIN );

	WallPaperChgFntOamPos( syswk->app );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �ǎ��ύX�F�ǎ��n�a�i�؂�ւ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_WallPaperObjChange( BOX2_SYS_WORK * syswk )
{
	NNSG2dCharacterData * chr;
	void * buf;
	u8 * cpy;
	u32	id;
	u32	wp;
	u32	i;

	wp = syswk->app->wallpaper_pos / BOX2OBJ_WPICON_MAX * BOX2OBJ_WPICON_MAX;
	id = BOX2OBJ_ID_WPICON;

	cpy = GFL_HEAP_AllocMemoryLo( HEAPID_BOX_APP, WP_ICON_CGX_SIZ );
	buf = GFL_ARC_UTIL_LoadOBJCharacter(
					ARCID_BOX2_GRA, NARC_box_gra_box_wp_image_NCGR, FALSE, &chr, HEAPID_BOX_APP );

	for( i=0; i<BOX2OBJ_WPICON_MAX; i++ ){
		GFL_STD_MemCopy( chr->pRawData, cpy, WP_ICON_CGX_SIZ );
		WallPaperBufferFill( syswk, cpy, wp, WP_ICON_CHG_COL, WP_ICON_CGX_SIZ );
		GFL_CLGRP_CGR_ReplaceEx(
			GFL_CLACT_WK_GetCgrIndex(syswk->app->clwk[id]), cpy, WP_ICON_CGX_SIZ, 0, CLSYS_DRAW_MAIN );
		id++;
		wp++;
	}

	GFL_HEAP_FreeMemory( buf );
	GFL_HEAP_FreeMemory( cpy );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ǎ��J���[�h��Ԃ�
 *
 * @param		buf				�L�����f�[�^
 * @param		wp				�ǎ��ԍ�
 * @param		chg_col		�ύX���J���[
 * @param		siz				�L�����T�C�Y
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WallPaperBufferFill( BOX2_SYS_WORK * syswk, u8 * buf, u32 wp, u32 chg_col, u32 siz )
{
	u32	i;
	u8	col;

	if( wp >= BOX_NORMAL_WALLPAPER_MAX ){
		if( BOXDAT_GetDaisukiKabegamiFlag( syswk->dat->sv_box, wp-BOX_NORMAL_WALLPAPER_MAX ) == FALSE ){
			col = WP_IMAGE_COLOR_START + BOX_TOTAL_WALLPAPER_MAX;
		}else{
			col = WP_IMAGE_COLOR_START + wp;
		}
	}else{
		col = WP_IMAGE_COLOR_START + wp;
	}

	for( i=0; i<siz; i++ ){
		if( buf[i] == chg_col ){
			buf[i] = col;
		}
	}
}


//============================================================================================
//	�|�P������a����{�b�N�X�I���֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �a����F�n�a�i������
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	id		OBJ ID
 * @param	x		�\���w���W
 * @param	y		�\���x���W
 * @param	flg		�\���t���O
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeOutBoxObjSet( BOX2_APP_WORK * appwk, u32 id, s16 x, s16 y, BOOL flg )
{
	BOX2OBJ_SetPos( appwk, id, x, y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_Vanish( appwk, id, flg );
	BOX2OBJ_BgPriChange( appwk, id, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * �a����F�n�a�i�������i�S�́j
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeOutBoxObjInit( BOX2_SYS_WORK * syswk )
{
	u32	i;
	s16	x, y;

//	PokeOutBoxObjSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, PTOUT_LA_PX, PTOUT_LA_PY, TRUE );
//	PokeOutBoxObjSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, PTOUT_RA_PX, PTOUT_RA_PY, TRUE );
	PokeOutBoxObjSet( syswk->app, BOX2OBJ_ID_TRAY_CUR, PTOUT_TRAY_CUR_PX, PTOUT_TRAY_CUR_PY, TRUE );
	PokeOutBoxObjSet( syswk->app, BOX2OBJ_ID_TRAY_NAME, PTOUT_TRAY_NAME_PX, PTOUT_TRAY_NAME_PY, TRUE );
//	PokeOutBoxObjSet( syswk->app, BOX2OBJ_ID_TRAY_ARROW, PTOUT_TRAY_ARROW_PX, PTOUT_TRAY_ARROW_PY, TRUE );

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, &x, &y, CLSYS_DEFREND_MAIN );
		PokeOutBoxObjSet( syswk->app, BOX2OBJ_ID_TRAYICON+i, x, PTOUT_TRAY_ICON_PY, TRUE );
	}

	BOX2OBJ_TrayIconChange( syswk );

	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_TRAY_NAME, BOX2OBJ_ANM_TRAY_NAME );

//	BoxMoveFntOamPos( syswk->app );
/*
	FONTOAM_SetDrawFlag( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, TRUE );
	FONTOAM_SetDrawFlag( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM].oam, TRUE );
	FONTOAM_SetBGPriority( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, 0 );
	FONTOAM_SetBGPriority( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM].oam, 0 );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �a����F�֘A�n�a�i���\����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeOutBoxObjVanish( BOX2_SYS_WORK * syswk )
{
	u32	i;

//	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_BOXMV_LA, FALSE );
//	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_BOXMV_RA, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_NAME, FALSE );
//	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_ARROW, FALSE );

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAYICON+i, FALSE );
	}
/*
	FONTOAM_SetDrawFlag( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, FALSE );
	FONTOAM_SetDrawFlag( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM].oam, FALSE );
*/
}

void BOX2OBJ_SetTouchBarButton( BOX2_SYS_WORK * syswk, u8 ret, u8 exit, u8 status )
{
	syswk->tb_ret_btn    = ret;
	syswk->tb_exit_btn   = exit;
	syswk->tb_status_btn = status;

	// �߂�
	if( ret == BOX2OBJ_TB_ICON_ON ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_CANCEL, TRUE );
		BOX2OBJ_AutoAnmSet( syswk->app, BOX2OBJ_ID_TB_CANCEL, APP_COMMON_BARICON_RETURN );
	}else if( ret == BOX2OBJ_TB_ICON_OFF ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_CANCEL, FALSE );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_CANCEL, TRUE );
		BOX2OBJ_AutoAnmSet( syswk->app, BOX2OBJ_ID_TB_CANCEL, APP_COMMON_BARICON_RETURN_OFF );
	}
	// ����
	if( exit == BOX2OBJ_TB_ICON_ON ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_END, TRUE );
		BOX2OBJ_AutoAnmSet( syswk->app, BOX2OBJ_ID_TB_END, APP_COMMON_BARICON_EXIT );
	}else if( exit == BOX2OBJ_TB_ICON_OFF ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_END, FALSE );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_END, TRUE );
		BOX2OBJ_AutoAnmSet( syswk->app, BOX2OBJ_ID_TB_END, APP_COMMON_BARICON_EXIT_OFF );
	}
	// �X�e�[�^�X
	if( status == BOX2OBJ_TB_ICON_ON ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_STATUS, TRUE );
		BOX2OBJ_AutoAnmSet( syswk->app, BOX2OBJ_ID_TB_STATUS, 0 );
	}else if( status == BOX2OBJ_TB_ICON_OFF ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_STATUS, FALSE );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_STATUS, TRUE );
		BOX2OBJ_AutoAnmSet( syswk->app, BOX2OBJ_ID_TB_STATUS, 2 );
	}
}

void BOX2OBJ_VanishTouchBarButton( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_CANCEL, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_END, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_STATUS, FALSE );
}



//============================================================================================
//	�{�b�N�X���Ɛ� ( OAM font )
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �n�`�l�t�H���g������
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_FontOamInit( BOX2_APP_WORK * appwk )
{
	BMPOAM_ACT_DATA	finit;
	BOX2_FONTOAM * fobj;
	u32	i;

	appwk->fntoam = BmpOam_Init( HEAPID_BOX_APP, appwk->clunit );

	// �{�b�N�X���i�{�b�N�X�ړ��p�j
	fobj = &appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NAME];

	fobj->bmp = GFL_BMP_Create( BOX2OBJ_FNTOAM_BOXNAME_SX, BOX2OBJ_FNTOAM_BOXNAME_SY, GFL_BMP_16_COLOR, HEAPID_BOX_APP );

	finit.bmp = fobj->bmp;
	finit.x = 0;
	finit.y = 0;
	finit.pltt_index = appwk->palRes[BOX2MAIN_PALRES_BOXOBJ];
	finit.pal_offset = 1;		// pltt_index�̃p���b�g���ł̃I�t�Z�b�g
	finit.soft_pri = 4;			// �\�t�g�v���C�I���e�B
	finit.bg_pri = 1;				// BG�v���C�I���e�B
//	finit.setSerface = CLSYS_DEFREND_MAIN;
	finit.setSerface = CLWK_SETSF_NONE;
	finit.draw_type  = CLSYS_DRAW_MAIN;

	fobj->oam = BmpOam_ActorAdd( appwk->fntoam, &finit );
	BmpOam_ActorSetDrawEnable( fobj->oam, FALSE );

	// �{�b�N�X���i�펞�\���j
	for( i=BOX2MAIN_FNTOAM_BOX_NAME1; i<=BOX2MAIN_FNTOAM_BOX_NAME2; i++ ){
		fobj = &appwk->fobj[i];
		fobj->bmp = GFL_BMP_Create( BOX2OBJ_FNTOAM_BOXNAME_SX, BOX2OBJ_FNTOAM_BOXNAME_SY, GFL_BMP_16_COLOR, HEAPID_BOX_APP );
		finit.bmp = fobj->bmp;
		finit.x = BOX2OBJ_BOXNAME_DPX;
		finit.y = BOX2OBJ_BOXNAME_DPY;
		finit.bg_pri = 3;				// BG�v���C�I���e�B
		fobj->oam = BmpOam_ActorAdd( appwk->fntoam, &finit );
	}

	// �i�[��
	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		fobj = &appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i];
		fobj->bmp = GFL_BMP_Create( BOX2OBJ_FNTOAM_BOXNUM_SX, BOX2OBJ_FNTOAM_BOXNUM_SY, GFL_BMP_16_COLOR, HEAPID_BOX_APP );
		finit.bmp = fobj->bmp;
		finit.x = BOX2OBJ_TRAYNUM_PX;
		finit.y = BOX2OBJ_TRAYNUM_PY+BOX2OBJ_TRAYNUM_SY*i;
		finit.bg_pri = 1;				// BG�v���C�I���e�B
		fobj->oam = BmpOam_ActorAdd( appwk->fntoam, &finit );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �n�`�l�t�H���g�폜
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_FontOamExit( BOX2_APP_WORK * appwk )
{
	u32	i;

	for( i=0; i<BOX2MAIN_FNTOAM_MAX; i++ ){
		BmpOam_ActorDel( appwk->fobj[i].oam );
		GFL_BMP_Delete( appwk->fobj[i].bmp );
	}
	BmpOam_Exit( appwk->fntoam );
}

void BOX2OBJ_FontOamVanish( BOX2_APP_WORK * appwk, u32 idx, BOOL flg )
{
	BmpOam_ActorSetDrawEnable( appwk->fobj[idx].oam, flg );
}

BOOL BOX2OBJ_CheckFontOamVanish( BOX2_APP_WORK * appwk, u32 idx )
{
	return BmpOam_ActorGetDrawEnable( appwk->fobj[idx].oam );
}


void BOX2OBJ_SetBoxNamePos( BOX2_APP_WORK * appwk, u32 idx, u32 mv )
{
	if( mv == BOX2MAIN_TRAY_SCROLL_L ){
		BmpOam_ActorSetPos( appwk->fobj[idx].oam, BOX2OBJ_BOXNAME_LPX, BOX2OBJ_BOXNAME_DPY );
	}else{
		BmpOam_ActorSetPos( appwk->fobj[idx].oam, BOX2OBJ_BOXNAME_RPX, BOX2OBJ_BOXNAME_DPY );
	}
}

void BOX2OBJ_BoxNameScroll( BOX2_APP_WORK * appwk, s8 mv )
{
	u32	i;
	s16	x, y;

	for( i=BOX2MAIN_FNTOAM_BOX_NAME1; i<=BOX2MAIN_FNTOAM_BOX_NAME2; i++ ){
		BmpOam_ActorGetPos( appwk->fobj[i].oam, &x, &y );
		BmpOam_ActorSetPos( appwk->fobj[i].oam, x+mv, y );
		if( x+mv == BOX2OBJ_BOXNAME_LPX || x+mv == BOX2OBJ_BOXNAME_RPX ){
			BOX2OBJ_FontOamVanish( appwk, i, FALSE );
		}
	}
}



//--------------------------------------------------------------------------------------------
/**
 * �n�`�l�t�H���g�ɂa�l�o���Đݒ�
 *
 * @param	appwk	�{�b�N�X��ʃA�v�����[�N
 * @param	win		�a�l�o�E�B���h�E
 * @param	id		�n�`�l�t�H���g�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_FontOamResetBmp( BOX2_APP_WORK * appwk, GFL_BMPWIN * win, u32 id )
{
	FONTOAM_OAM_DATA_PTR dat = FONTOAM_OAMDATA_Make( win, HEAPID_BOX_APP );

	FONTOAM_OAMDATA_ResetBmp( appwk->fobj[id].oam, dat, win, HEAPID_BOX_APP );

	FONTOAM_OAMDATA_Free( dat );
}
*/
