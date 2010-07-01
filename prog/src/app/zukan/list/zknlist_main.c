//============================================================================================
/**
 * @file		zknlist_main.c
 * @brief		�}�Ӄ��X�g��� ���C������
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	���W���[�����FZKNLISTMAIN
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"
#include "pokeicon/pokeicon.h"
#include "print/global_msg.h"
#include "font/font.naix"
#include "app/app_menu_common.h"
#include "msg/msg_zukan_list.h"
#include "message.naix"

#include "zknlist_main.h"
#include "zknlist_bmp.h"
#include "zknlist_obj.h"
#include "zknlist_snd_def.h"
#include "zukan_gra.naix"
#include "zukan_data.naix"


//============================================================================================
//	�萔��`
//============================================================================================
#define	EXP_BUF_SIZE	( 128 )		// �e���|�������b�Z�[�W�o�b�t�@�T�C�Y

#define	BLEND_EV1		( 6 )				// �u�����h�W��
#define	BLEND_EV2		( 10 )			// �u�����h�W��

#define	FRAME_SCROLL_COUNT	( 48 )		// �w�i�X�N���[���J�E���g


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void VBlankTask( GFL_TCB * tcb, void * work );
static void HBlankTask( GFL_TCB * tcb, void * work );

static void ListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void ListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void ListCallBack_Scroll( void * work, s8 mv );


//============================================================================================
//	�O���[�o��
//============================================================================================

// VRAM����U��
static const GFL_DISP_VRAM VramTbl = {
	GX_VRAM_BG_128_A,							// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,				// ���C��2D�G���W����BG�g���p���b�g

	GX_VRAM_SUB_BG_128_C,					// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

	GX_VRAM_OBJ_128_B,						// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g

	GX_VRAM_SUB_OBJ_128_D,					// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,		// �T�u2D�G���W����OBJ�g���p���b�g

	GX_VRAM_TEX_NONE,							// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_NONE,					// �e�N�X�`���p���b�g�X���b�g

	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_128K		// �T�uOBJ�}�b�s���O���[�h
};

// ���X�g�R�[���o�b�N
static const FRAMELIST_CALLBACK	FRMListCallBack = {
	ListCallBack_Draw,
	ListCallBack_Move,
	ListCallBack_Scroll,
};

// ���X�g�^�b�`�f�[�^
static const FRAMELIST_TOUCH_DATA TouchHitTbl[] =
{
	{ {   0,  23, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 00: �|�P�����A�C�R��
	{ {  24,  47, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 01: �|�P�����A�C�R��
	{ {  48,  71, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 02: �|�P�����A�C�R��
	{ {  72,  95, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 03: �|�P�����A�C�R��
	{ {  96, 119, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 04: �|�P�����A�C�R��
	{ { 120, 143, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 05: �|�P�����A�C�R��
	{ { 144, 167, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 06: �|�P�����A�C�R��

	{ {   8, 160, 232, 255 }, FRAMELIST_TOUCH_PARAM_RAIL },					// 07: ���[��

	{ { 168, 191, 136, 159 }, FRAMELIST_TOUCH_PARAM_PAGE_UP },			// 08: ��
	{ { 168, 191, 160, 183 }, FRAMELIST_TOUCH_PARAM_PAGE_DOWN },		// 09: �E

	{ { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};

// ���X�g�w�b�_�[�f�[�^
static const FRAMELIST_HEADER	ListHeader = {
	GFL_BG_FRAME2_M,							// ����ʂa�f
	GFL_BG_FRAME2_S,							// ���ʂa�f

	ZKNLISTMAIN_LIST_PX,					// ���ڃt���[���\���J�n�w���W
	ZKNLISTMAIN_LIST_PY,					// ���ڃt���[���\���J�n�x���W
	ZKNLISTMAIN_LIST_SX,					// ���ڃt���[���\���w�T�C�Y
	ZKNLISTMAIN_LIST_SY,					// ���ڃt���[���\���x�T�C�Y

	7,							// �t���[�����ɕ\������BMPWIN�̕\���w���W
	0,							// �t���[�����ɕ\������BMPWIN�̕\���x���W
	11,							// �t���[�����ɕ\������BMPWIN�̕\���w�T�C�Y
	3,							// �t���[�����ɕ\������BMPWIN�̕\���x�T�C�Y
	1,							// �t���[�����ɕ\������BMPWIN�̃p���b�g

	{ 24, 12, 8, 6, 4, 3 },		// �X�N���[�����x [0] = �ő�

	3,							// �I�����ڂ̃p���b�g

	8,							// �X�N���[���o�[�̂x�T�C�Y

	0,							// ���ړo�^��
	3,							// �w�i�o�^��

	0,												// �����ʒu
	ZKNLISTMAIN_LIST_POS_MAX,	// �J�[�\���ړ��͈�
	0,												// �����X�N���[���l

	TouchHitTbl,			// �^�b�`�f�[�^

	&FRMListCallBack,	// �R�[���o�b�N�֐�
	NULL,
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��ݒ�
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitVBlank( ZKNLISTMAIN_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask, wk, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��폜
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_ExitVBlank( ZKNLISTMAIN_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK����
 *
 * @param		tcb			GFL_TCB
 * @param		work		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankTask( GFL_TCB * tcb, void * work )
{
	ZKNLISTMAIN_WORK * wk = work;

	GFL_BG_VBlankFunc();
	GFL_CLACT_SYS_VBlankFunc();

	PaletteFadeTrans( wk->pfd );

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		HBLANK�֐��ݒ�
 *
 * @param		wk			�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitHBlank( ZKNLISTMAIN_WORK * wk )
{
	wk->htask = GFUser_HIntr_CreateTCB( HBlankTask, wk, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		HBLANK�֐��폜
 *
 * @param		wk			�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_ExitHBlank( ZKNLISTMAIN_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->htask );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		HBLANK����
 *
 * @param		tcb			GFL_TCB
 * @param		work		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void HBlankTask( GFL_TCB * tcb, void * work )
{
	s32	vcount = GX_GetVCount();

	if( vcount >= 168 ){
		G2S_ChangeBlendAlpha( 11, 5 );
	}else if( vcount >= 144 ){
		G2S_ChangeBlendAlpha( 10, 6 );
	}else if( vcount >= 120 ){
		G2S_ChangeBlendAlpha( 9, 7 );
	}else if( vcount >= 96 ){
		G2S_ChangeBlendAlpha( 8, 8 );
	}else if( vcount >= 72 ){
		G2S_ChangeBlendAlpha( 6, 10 );
	}else{
		G2S_ChangeBlendAlpha( 4, 12 );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VRAM������
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitVram(void)
{
	GFL_DISP_SetBank( &VramTbl );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VRAM�ݒ�擾
 *
 * @param		none
 *
 * @return	VRAM�ݒ�f�[�^
 */
//--------------------------------------------------------------------------------------------
const GFL_DISP_VRAM * ZKNLISTMAIN_GetVramBankData(void)
{
	return &VramTbl;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f������
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitBg(void)
{
	GFL_BG_Init( HEAPID_ZUKAN_LIST );

	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

	{	// ���C����ʁF�J�o�[�H
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME0_M );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0, HEAPID_ZUKAN_LIST );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_M );
	}
	{	// ���C����ʁF�^�b�`�o�[
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x4000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// ���C����ʁF���X�g
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME2_M );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_M );
	}
	{	// ���C����ʁF�w�i
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x18000, 0x8000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &cnth, GFL_BG_MODE_TEXT );
	}

	{	// �T�u��ʁF�J�o�[�H
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME0_S );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_S, 0x20, 0, HEAPID_ZUKAN_LIST );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_S );
	}
	{	// �T�u��ʁF�t���[��
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x18000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// �T�u��ʁF���X�g
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME2_S );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
	}

	{	// �T�u��ʁF�w�i
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x18000, 0x8000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &cnth, GFL_BG_MODE_TEXT );
	}

	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_ON );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f���
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_ExitBg(void)
{
	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_OFF );

	GFL_BG_FreeBGControl( GFL_BG_FRAME3_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

	GFL_BG_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�O���t�B�b�N�ǂݍ���
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_LoadBgGraphic(void)
{
	ARCHANDLE * ah;

	ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_zukan_gra_list_list_bgd_NCLR, PALTYPE_MAIN_BG, 0, 0x20*4, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_zukan_gra_list_list_bgu_NCLR, PALTYPE_SUB_BG, 0, 0x20*5, HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_list_listframe_bgd_NCGR, GFL_BG_FRAME2_M, 0, 0, FALSE, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_list_listbase_bgd_NCGR, GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_list_listframe_bgu_NCGR, GFL_BG_FRAME2_S, 0, 0, FALSE, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_list_listbase_bgu_NCGR, GFL_BG_FRAME3_S, 0, 0, FALSE, HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_list_listbase_bgd_NSCR,
		GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_list_listtitle_bgu_NSCR,
		GFL_BG_FRAME1_S, 0, 0, FALSE, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_list_listbase_bgu_NSCR,
		GFL_BG_FRAME3_S, 0, 0, FALSE, HEAPID_ZUKAN_LIST );

	GFL_ARC_CloseDataHandle( ah );

	// �^�b�`�o�[
	ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
		ah, APP_COMMON_GetBarCharArcIdx(), GFL_BG_FRAME1_M, 0, FALSE, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, APP_COMMON_GetBarPltArcIdx(), PALTYPE_MAIN_BG,
		ZKNLISTMAIN_MBG_PAL_TOUCHBAR*0x20, 0x20, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, APP_COMMON_GetBarScrnArcIdx(),
		GFL_BG_FRAME1_M, 0, 0, FALSE, HEAPID_ZUKAN_LIST );
	GFL_BG_ChangeScreenPalette(
		GFL_BG_FRAME1_M, ZKNLISTMAIN_TOUCH_BAR_PX, ZKNLISTMAIN_TOUCH_BAR_PY,
		ZKNLISTMAIN_TOUCH_BAR_SX, ZKNLISTMAIN_TOUCH_BAR_SY, ZKNLISTMAIN_MBG_PAL_TOUCHBAR );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_M );

	GFL_ARC_CloseDataHandle( ah );

	// �t�H���g�p���b�g
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
		ZKNLISTMAIN_MBG_PAL_FONT*0x20, 0x20, HEAPID_ZUKAN_LIST );
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
		ZKNLISTMAIN_SBG_PAL_FONT*0x20, 0x20, HEAPID_ZUKAN_LIST );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h������
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitPaletteFade( ZKNLISTMAIN_WORK * wk )
{
	wk->pfd = PaletteFadeInit( HEAPID_ZUKAN_LIST );

	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_BG, FADE_PAL_ALL_SIZE, HEAPID_ZUKAN_LIST );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_SUB_BG, FADE_PAL_ALL_SIZE, HEAPID_ZUKAN_LIST );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_OBJ, FADE_PAL_ALL_SIZE, HEAPID_ZUKAN_LIST );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_SUB_OBJ, FADE_PAL_ALL_SIZE, HEAPID_ZUKAN_LIST );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h���
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_ExitPaletteFade( ZKNLISTMAIN_WORK * wk )
{
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_BG );
	PaletteFadeWorkAllocFree( wk->pfd, FADE_SUB_BG );
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_OBJ );
	PaletteFadeWorkAllocFree( wk->pfd, FADE_SUB_OBJ );

	PaletteFadeFree( wk->pfd );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p���b�g�t�F�[�h���N�G�X�g
 *
 * @param		wk			�}�Ӄ��X�g���[�N
 * @param		start		�J�n�Z�x
 * @param		end			�I���Z�x
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_SetPalFadeSeq( ZKNLISTMAIN_WORK * wk, u8 start, u8 end )
{
	PaletteFadeReq( wk->pfd, PF_BIT_MAIN_BG,  0xbfff, ZKNCOMM_FADE_WAIT, start, end, 0, GFUser_VIntr_GetTCBSYS() );
	PaletteFadeReq( wk->pfd, PF_BIT_SUB_BG,   0xffef, ZKNCOMM_FADE_WAIT, start, end, 0, GFUser_VIntr_GetTCBSYS() );
	PaletteFadeReq( wk->pfd, PF_BIT_MAIN_OBJ, 0xf878, ZKNCOMM_FADE_WAIT, start, end, 0, GFUser_VIntr_GetTCBSYS() );
	PaletteFadeReq( wk->pfd, PF_BIT_SUB_OBJ,  0xbfff, ZKNCOMM_FADE_WAIT, start, end, 0, GFUser_VIntr_GetTCBSYS() );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�A���t�@�u�����h�ݒ�
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_SetBlendAlpha(void)
{
	G2S_SetBlendAlpha(
		GX_BLEND_PLANEMASK_BG2,
		GX_BLEND_PLANEMASK_BG3,
		BLEND_EV1, BLEND_EV2 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�E�B���h�E�ݒ�
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_SetWindow(void)
{
	G2S_SetWndOBJInsidePlane(
		GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 |
		GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE );
	G2S_SetWndOutsidePlane(
		GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 |
		GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE );
	GXS_SetVisibleWnd( GX_WNDMASK_OW );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�E�B���h�E����
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_ResetWindow(void)
{
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A������
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitMsg( ZKNLISTMAIN_WORK * wk )
{
	wk->mman = GFL_MSG_Create(
							GFL_MSG_LOAD_NORMAL,
							ARCID_MESSAGE, NARC_message_zukan_list_dat, HEAPID_ZUKAN_LIST );
	wk->font = GFL_FONT_Create(
												ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_ZUKAN_LIST );
	wk->wset = WORDSET_Create( HEAPID_ZUKAN_LIST );
	wk->que  = PRINTSYS_QUE_Create( HEAPID_ZUKAN_LIST );
	wk->exp  = GFL_STR_CreateBuffer( EXP_BUF_SIZE, HEAPID_ZUKAN_LIST );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A���
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_ExitMsg( ZKNLISTMAIN_WORK * wk )
{
	GFL_STR_DeleteBuffer( wk->exp );
	PRINTSYS_QUE_Delete( wk->que );
	WORDSET_Delete( wk->wset );
	GFL_FONT_Delete( wk->font );
	GFL_MSG_Delete( wk->mman );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �n���}�Ӕԍ����X�g�쐬
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_LoadLocalNoList( ZKNLISTMAIN_WORK * wk )
{
	wk->localNo = POKE_PERSONAL_GetZenkokuToChihouArray( HEAPID_ZUKAN_LIST, NULL );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �n���}�Ӕԍ����X�g���
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_FreeLocalNoList( ZKNLISTMAIN_WORK * wk )
{
	GFL_HEAP_FreeMemory( wk->localNo );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �^�u�t���[�������ʒu�ݒ�
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitFrameScroll( ZKNLISTMAIN_WORK * wk )
{
	s16	x, y;
	u32	i;

	GFL_BG_SetScrollReq( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, FRAME_SCROLL_COUNT );
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_SET, -FRAME_SCROLL_COUNT );

	for( i=ZKNLISTOBJ_IDX_TB_RETURN; i<=ZKNLISTOBJ_IDX_TB_START; i++ ){
		ZKNLISTOBJ_GetPos( wk, i, &x, &y, CLSYS_DRAW_MAIN );
		y += FRAME_SCROLL_COUNT;
		ZKNLISTOBJ_SetPos( wk, i, x, y, CLSYS_DRAW_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �^�u�t���[���X�N���[���ݒ�
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_SetFrameScrollParam( ZKNLISTMAIN_WORK * wk, s16 val )
{
	wk->frameScrollCnt = FRAME_SCROLL_COUNT;
	wk->frameScrollVal = val;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �^�u�t���[���X�N���[��
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @retval	"TRUE = �X�N���[����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZKNLISTMAIN_MainFrameScroll( ZKNLISTMAIN_WORK * wk )
{
	s16	x, y;
	u32	i;

	if( wk->frameScrollCnt == 0 ){
		return FALSE;
	}

	GFL_BG_SetScrollReq( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_INC, wk->frameScrollVal );
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_DEC, wk->frameScrollVal );

	for( i=ZKNLISTOBJ_IDX_TB_RETURN; i<=ZKNLISTOBJ_IDX_TB_START; i++ ){
		ZKNLISTOBJ_GetPos( wk, i, &x, &y, CLSYS_DRAW_MAIN );
		y += wk->frameScrollVal;
		ZKNLISTOBJ_SetPos( wk, i, x, y, CLSYS_DRAW_MAIN );
	}

	wk->frameScrollCnt -= GFL_STD_Abs(wk->frameScrollVal);

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���X�g�쐬
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_MakeList( ZKNLISTMAIN_WORK * wk )
{
	ARCHANDLE * ah;
	u32	i;

	ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_LIST_L );

	// ���X�g���[�N�쐬
	{
		FRAMELIST_HEADER	hed;
		u16	def_mons;

		def_mons = ZUKANSAVE_GetDefaultMons( wk->dat->savedata );

		hed = ListHeader;
		hed.itemMax = MONSNO_END;
		hed.cbWork  = wk;

		for( i=0; i<wk->dat->listMax; i++ ){
			if( wk->dat->list[i] == def_mons ){
				if( i > (wk->dat->listMax-ZKNLISTMAIN_LIST_POS_MAX) ){
					hed.initPos    = i - ( wk->dat->listMax - ZKNLISTMAIN_LIST_POS_MAX );
					hed.initScroll = i - hed.initPos;
				}else if( i < ZKNLISTMAIN_LIST_POS_MAX ){
					hed.initPos    = i;
				}else{
					hed.initScroll = i;
				}
				break;
			}
		}

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_LIST );
	}

	// �\�����ڐ�
	if( wk->dat->listMax < ZKNLISTMAIN_LIST_POS_MAX ){
		wk->listPosMax = wk->dat->listMax;
	}else{
		wk->listPosMax = ZKNLISTMAIN_LIST_POS_MAX;
	}

	// ���ڔw�i�ݒ�
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_list_listframe_NSCR, FALSE, 0 );
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_list_listframe2_NSCR, FALSE, 1 );
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_list_listframe3_NSCR, FALSE, 2 );

	// �_�ŃA�j���p���b�g�ݒ�
	FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_zukan_gra_list_list_bgd_NCLR, 2, 3 );

	GFL_ARC_CloseDataHandle( ah );

	// ���X�g�p�����[�^�ݒ�E���ڕ�����쐬
	for( i=0; i<wk->dat->listMax; i++ ){
		if( ZUKANSAVE_GetPokeGetFlag( wk->dat->savedata, wk->dat->list[i] ) == TRUE ){
			STRBUF * srcStr = GFL_MSG_CreateString( GlobalMsg_PokeName, wk->dat->list[i] );
			wk->name[i] = GFL_STR_CreateCopyBuffer( srcStr, HEAPID_ZUKAN_LIST );
			GFL_STR_DeleteBuffer( srcStr );
			FRAMELIST_AddItem( wk->lwk, 0, SET_LIST_PARAM(ZUKAN_LIST_MONS_GET,wk->dat->list[i]) );
		}else if( ZUKANSAVE_GetPokeSeeFlag( wk->dat->savedata, wk->dat->list[i] ) == TRUE ){
			STRBUF * srcStr = GFL_MSG_CreateString( GlobalMsg_PokeName, wk->dat->list[i] );
			wk->name[i] = GFL_STR_CreateCopyBuffer( srcStr, HEAPID_ZUKAN_LIST );
			GFL_STR_DeleteBuffer( srcStr );
			FRAMELIST_AddItem( wk->lwk, 1, SET_LIST_PARAM(ZUKAN_LIST_MONS_SEE,wk->dat->list[i]) );
		}else{
			wk->name[i] = GFL_MSG_CreateString( wk->mman, str_name_01 );
			FRAMELIST_AddItem( wk->lwk, 2, SET_LIST_PARAM(ZUKAN_LIST_MONS_NONE,wk->dat->list[i]) );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���X�g�폜
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_FreeList( ZKNLISTMAIN_WORK * wk )
{
	u32	i;

	FRAMELIST_Exit( wk->lwk );

	// ���O�폜
	for( i=0; i<MONSNO_END; i++ ){
		if( wk->name[i] != NULL ){
			GFL_STR_DeleteBuffer( wk->name[i] );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���X�g�R�[���o�b�N�F���ڕ\��
 *
 * @param		work			�}�Ӄ��X�g���[�N
 * @param		itemNum		���ڔԍ�
 * @param		util			���ڂŎg�p���Ă���PRINT_UTIL
 * @param		py				�\���x���W
 * @param		disp			�`���
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNLISTMAIN_WORK * wk;
	u32	prm;
	u32	obj;

	wk = work;

	prm = FRAMELIST_GetItemParam( wk->lwk, itemNum );

	ZKNLISTBMP_PutPokeList( wk, util, wk->name[itemNum], GET_LIST_MONS(prm) );

	if( GET_LIST_INFO(prm) != 0 ){
		ZKNLISTOBJ_PutPokeList( wk, GET_LIST_MONS(prm), py, disp );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���X�g�R�[���o�b�N�F�J�[�\���ړ�
 *
 * @param		work			�}�Ӄ��X�g���[�N
 * @param		listPos		���X�g�ʒu
 * @param		flg				FALSE = ��������
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
	ZKNLISTMAIN_WORK * wk;
	u32	prm;

	wk = work;

	prm = FRAMELIST_GetItemParam( wk->lwk, listPos );

	if( GET_LIST_INFO(prm) != 0 ){
		ZKNLISTOBJ_SetPokeGra( wk, GET_LIST_MONS(prm) );
	}else{
		ZKNLISTOBJ_SetPokeGra( wk, 0 );
	}
	ZKNLISTOBJ_ChangePokeIconAnime( wk, listPos-FRAMELIST_GetScrollCount(wk->lwk) );
//	ZKNLISTOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY2(wk->lwk) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���X�g�R�[���o�b�N�F���X�g�X�N���[��
 *
 * @param		work			�}�Ӄ��X�g���[�N
 * @param		mv				�X�N���[���l
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ListCallBack_Scroll( void * work, s8 mv )
{
	ZKNLISTMAIN_WORK * wk;

	wk = work;

	ZKNLISTOBJ_PutScrollList( wk, mv*-1 );
	ZKNLISTOBJ_ChangePokeIconAnime( wk, FRAMELIST_GetCursorPos(wk->lwk) );

//	ZKNLISTOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY2(wk->lwk) );
	ZKNLISTOBJ_SetListPageArrowAnime( wk, TRUE );
}
