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
#include "zknlist_bgwfrm.h"
#include "zknlist_snd_def.h"
#include "zukan_gra.naix"
#include "zukan_data.naix"


//============================================================================================
//	�萔��`
//============================================================================================
#define	EXP_BUF_SIZE	( 128 )		// �e���|�������b�Z�[�W�o�b�t�@�T�C�Y

#define	BLEND_EV1		( 6 )				// �u�����h�W��
#define	BLEND_EV2		( 10 )			// �u�����h�W��


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void VBlankTask( GFL_TCB * tcb, void * work );
static void HBlankTask( GFL_TCB * tcb, void * work );
static void ListCallBack( void * work, s16 nowPos, s16 nowScroll, s16 oldPos, s16 oldScroll, u32 mv );


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
 * @param		wk			�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankTask( GFL_TCB * tcb, void * work )
{
//	ZKNLISTMAIN_WORK * wk = work;

	GFL_BG_VBlankFunc();
	GFL_CLACT_SYS_VBlankFunc();

//	PaletteFadeTrans( syswk->app->pfd );

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

void ZKNLISTMAIN_InitHBlank( ZKNLISTMAIN_WORK * wk )
{
	wk->htask = GFUser_HIntr_CreateTCB( HBlankTask, wk, 0 );
}

void ZKNLISTMAIN_ExitHBlank( ZKNLISTMAIN_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->htask );
}

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



void ZKNLISTMAIN_InitVram(void)
{
	GFL_DISP_SetBank( &VramTbl );
}

const GFL_DISP_VRAM * ZKNLISTMAIN_GetVramBankData(void)
{
	return &VramTbl;
}


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

void ZKNLISTMAIN_LoadBgGraphic(void)
{
	ARCHANDLE * ah;

	ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_zukan_gra_list_list_bgd_NCLR, PALTYPE_MAIN_BG, 0, 0x20*4, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_zukan_gra_list_list_bgu_NCLR, PALTYPE_SUB_BG, 0, 0x20*4, HEAPID_ZUKAN_LIST );

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



enum {
	LIST_INFO_MONS_NONE = 0,
	LIST_INFO_MONS_SEE,
	LIST_INFO_MONS_GET,
};


//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���X�g�쐬
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void ListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void ListCallBack_Scroll( void * work, s8 mv );

static const FRAMELIST_CALLBACK	FRMListCallBack = {
	ListCallBack_Draw,
	ListCallBack_Move,
	ListCallBack_Scroll,
};

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

static const FRAMELIST_HEADER	ListHeader = {
	GFL_BG_FRAME2_M,							// ����ʂa�f
	GFL_BG_FRAME2_S,							// ���ʂa�f

	ZKNLISTMAIN_LIST_PX,					// ���ڃt���[���\���J�n�w���W
	ZKNLISTMAIN_LIST_PY,					// ���ڃt���[���\���J�n�x���W
	ZKNLISTMAIN_LIST_SX,					// ���ڃt���[���\���w�T�C�Y
	ZKNLISTMAIN_LIST_SY,					// ���ڃt���[���\���x�T�C�Y

	3,							// �t���[�����ɕ\������BMPWIN�̕\���w���W
	0,							// �t���[�����ɕ\������BMPWIN�̕\���x���W
	11,							// �t���[�����ɕ\������BMPWIN�̕\���w�T�C�Y
	3,							// �t���[�����ɕ\������BMPWIN�̕\���x�T�C�Y
	1,							// �t���[�����ɕ\������BMPWIN�̃p���b�g

	{ 24, 12, 8, 6, 4, 3 },		// �X�N���[�����x [0] = �ő�

	3,							// �I�����ڂ̃p���b�g

	8,							// �X�N���[���o�[�̂x�T�C�Y

	0,							// ���ړo�^��
	2,							// �w�i�o�^��

	0,							// �����ʒu
	7,							// �J�[�\���ړ��͈�
	0,							// �����X�N���[���l

	TouchHitTbl,			// �^�b�`�f�[�^

	&FRMListCallBack,	// �R�[���o�b�N�֐�
	NULL,
};

static void ListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNLISTMAIN_WORK * wk;
	u32	prm;
	u32	obj;

	wk = work;

	prm = FRAMELIST_GetItemParam( wk->lwk, itemNum );

	ZKNLISTBMP_PutPokeList2( wk, util, wk->name[itemNum], GET_LIST_MONS(prm) );

	if( GET_LIST_INFO(prm) != 0 ){
		ZKNLISTOBJ_PutPokeList2( wk, GET_LIST_MONS(prm), py, disp );
	}
}

static void ListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
	ZKNLISTMAIN_WORK * wk;
	u32	prm;

	wk = work;

	prm = FRAMELIST_GetItemParam( wk->lwk, listPos );
//	OS_Printf( "info = %d, mons = %d\n", GET_LIST_INFO(prm), GET_LIST_MONS(prm) );

	if( GET_LIST_INFO(prm) != 0 ){
		ZKNLISTOBJ_SetPokeGra( wk, GET_LIST_MONS(prm) );
	}else{
		ZKNLISTOBJ_SetPokeGra( wk, 0 );
	}
	ZKNLISTOBJ_ChangePokeIconAnime( wk, listPos-FRAMELIST_GetScrollCount(wk->lwk) );
	ZKNLISTOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );

/*
	if( flg == TRUE ){
		PMSND_PlaySE( ZKNLIST_SE_MOVE );
	}
*/
}

static void ListCallBack_Scroll( void * work, s8 mv )
{
	ZKNLISTMAIN_WORK * wk;

	wk = work;

	ZKNLISTOBJ_PutScrollList2( wk, mv*-1 );
	ZKNLISTOBJ_ChangePokeIconAnime( wk, FRAMELIST_GetCursorPos(wk->lwk) );

	ZKNLISTOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
	ZKNLISTOBJ_SetListPageArrowAnime( wk, TRUE );

//	PMSND_PlaySE( ZKNLIST_SE_SCROLL );
}


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
				if( i > (wk->dat->listMax-7) ){
					hed.initPos    = i - ( wk->dat->listMax - 7 );
					hed.initScroll = i - hed.initPos;
				}else{
					hed.initScroll = i;
				}
			}
		}

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_LIST );
	}

	// ���ڔw�i�ݒ�
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_list_listframe_NSCR, FALSE, 0 );
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_list_listframe2_NSCR, FALSE, 1 );

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
			FRAMELIST_AddItem( wk->lwk, 1, SET_LIST_PARAM(ZUKAN_LIST_MONS_NONE,wk->dat->list[i]) );
		}
	}



/*
	// ��������
	wk->seeNum = ;
	// �߂܂�����
	wk->getNum = ;
*/

/*
extern u16 ZUKANSAVE_GetPokeGetCount(const ZUKAN_SAVEDATA * zs);
extern u16 ZUKANSAVE_GetPokeSeeCount(const ZUKAN_SAVEDATA * zs);

extern u16 ZUKANSAVE_GetLocalPokeGetCount(const ZUKAN_SAVEDATA * zs);
extern u16 ZUKANSAVE_GetLocalPokeSeeCount(const ZUKAN_SAVEDATA * zs);
*/

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

//	BLINKPALANM_Exit( wk->blink );
//	ZKNLISTMAIN_ExitList( wk->list );
}


void ZKNLISTMAIN_LoadLocalNoList( ZKNLISTMAIN_WORK * wk )
{
	wk->localNo = GFL_ARC_UTIL_Load(
									ARCID_ZUKAN_DATA, NARC_zukan_data_zkn_chihou_no_dat, FALSE, HEAPID_ZUKAN_LIST );
}

void ZKNLISTMAIN_FreeLocalNoList( ZKNLISTMAIN_WORK * wk )
{
	GFL_HEAP_FreeMemory( wk->localNo );
}



