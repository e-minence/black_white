//============================================================================================
/**
 * @file		ddemo_main.c
 * @brief		�a������f�����
 * @author	Hiroyuki Nakamura
 * @date		10.04.06
 *
 *	���W���[�����FDDEMOMAIN
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"
#include "sound/pm_wb_voice.h"
#include "font/font.naix"

#include "ddemo_main.h"
#include "dendou_demo_gra.naix"


//============================================================================================
//	�萔��`
//============================================================================================
#define	EXP_BUF_SIZE	( 1024 )	// �e���|�������b�Z�[�W�o�b�t�@�T�C�Y

#define DTCM_SIZE		(0x1000)		// �W�I���g���o�b�t�@�̎g�p�T�C�Y


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void VBlankTask( GFL_TCB * tcb, void * work );


//============================================================================================
//	�O���[�o��
//============================================================================================

// VRAM����U��
static const GFL_DISP_VRAM VramTbl = {
	GX_VRAM_BG_128_D,							// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_23_G,				// ���C��2D�G���W����BG�g���p���b�g

	GX_VRAM_SUB_BG_128_C,					// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_0123_H,	// �T�u2D�G���W����BG�g���p���b�g

	GX_VRAM_OBJ_128_B,						// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g

	GX_VRAM_SUB_OBJ_16_I,					// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g

	GX_VRAM_TEX_0_A,							// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_0123_E,				// �e�N�X�`���p���b�g�X���b�g

	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_128K		// �T�uOBJ�}�b�s���O���[�h
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��ݒ�
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_InitVBlank( DDEMOMAIN_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask, wk, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��폜
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_ExitVBlank( DDEMOMAIN_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK����
 *
 * @param		tcb		GFL_TCB
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankTask( GFL_TCB * tcb, void * work )
{
	DDEMOMAIN_WORK * wk = work;
/*
	PaletteFadeTrans( wk->pfd );
*/
	GFL_BG_VBlankFunc();
	GFL_CLACT_SYS_VBlankFunc();
	if( wk->double3dFlag == TRUE ){
		GFL_G3D_DOUBLE3D_VblankIntr();
	}
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

void DDEMOMAIN_InitVram(void)
{
	GFL_DISP_ClearVRAM( 0 );
	GFL_DISP_SetBank( &VramTbl );
}

const GFL_DISP_VRAM * DDEMOMAIN_GetVramBankData(void)
{
	return &VramTbl;
}

void DDEMOMAIN_InitBg(void)
{
	GFL_BG_Init( HEAPID_DENDOU_DEMO );

	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BGMODE_3, GX_BG0_AS_3D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

/*
	{	// ���C����ʁF�w�i
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// ���C����ʁF�X�|�b�g���C�g
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// ���C����ʁF�^�C�g���w�i
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// ���C����ʁF����
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &cnth, GFL_BG_MODE_TEXT );
	}

	{	// �T�u��ʁF�w�i
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// �T�u��ʁF����
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &cnth, GFL_BG_MODE_TEXT );
	}

	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1, VISIBLE_ON );
*/

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
}

void DDEMOMAIN_ExitBg(void)
{
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );

/*
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
*/

	GFL_BG_Exit();
}

void DDEMOMAIN_Init2ndSceneBgFrame(void)
{
	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BGMODE_3, GX_BG0_AS_3D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

	{
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GX_BG_SCRSIZE_256x16PLTT_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x10000, 0x10000,
			GX_BG_EXTPLTT_23, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &cnth, GFL_BG_MODE_256X16 );
		GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &cnth, GFL_BG_MODE_256X16 );
	}

	GFL_BG_ClearScreen( GFL_BG_FRAME3_M );
	GFL_BG_ClearScreen( GFL_BG_FRAME3_S );

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
}

void DDEMOMAIN_Exit2ndSceneBgFrame(void)
{
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );

	GFL_BG_FreeBGControl( GFL_BG_FRAME3_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
}

void DDEMOMAIN_Load2ndBgGraphic(void)
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_DENDOU_DEMO_GRA, HEAPID_DENDOU_DEMO );

	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_dendou_demo_gra_bg_s2_NCLR, PALTYPE_MAIN_BG_EX, 0x6000, 0x20, HEAPID_DENDOU_DEMO );
	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_dendou_demo_gra_bg_s2_NCLR, PALTYPE_SUB_BG_EX, 0x6000, 0x20, HEAPID_DENDOU_DEMO );

	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_dendou_demo_gra_bg_s2_NCGR, GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_DENDOU_DEMO );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_dendou_demo_gra_bg_s2_NCGR, GFL_BG_FRAME3_S, 0, 0, FALSE, HEAPID_DENDOU_DEMO );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_dendou_demo_gra_bgd_s2_NSCR, GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_DENDOU_DEMO );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_dendou_demo_gra_bgd_s2_NSCR, GFL_BG_FRAME3_S, 0, 0, FALSE, HEAPID_DENDOU_DEMO );

	GFL_ARC_CloseDataHandle( ah );
}

void DDEMOMAIN_SetBlendAlpha(void)
{
	G2_SetBlendAlpha(
		GX_BLEND_PLANEMASK_BG0,
		GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD,
		6, 10 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A������
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_InitMsg( DDEMOMAIN_WORK * wk )
{
	wk->mman = GFL_MSG_Create(
							GFL_MSG_LOAD_NORMAL,
							ARCID_MESSAGE, NARC_message_dendou_demo_dat, HEAPID_DENDOU_DEMO );
	wk->font = GFL_FONT_Create(
							ARCID_FONT, NARC_font_large_gftr,
							GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_DENDOU_DEMO );
  wk->nfnt = GFL_FONT_Create(
							ARCID_FONT, NARC_font_num_gftr,
							GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_DENDOU_DEMO );
	wk->wset = WORDSET_Create( HEAPID_DENDOU_DEMO );
//	wk->que  = PRINTSYS_QUE_Create( HEAPID_DENDOU_DEMO );
	wk->exp  = GFL_STR_CreateBuffer( EXP_BUF_SIZE, HEAPID_DENDOU_DEMO );

//  PRINTSYS_QUE_ForceCommMode( wk->que, TRUE );      // �e�X�g
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A���
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_ExitMsg( DDEMOMAIN_WORK * wk )
{
	GFL_STR_DeleteBuffer( wk->exp );
//	PRINTSYS_QUE_Delete( wk->que );
	WORDSET_Delete( wk->wset );
	GFL_FONT_Delete( wk->nfnt );
	GFL_FONT_Delete( wk->font );
	GFL_MSG_Delete( wk->mman );
}

static const u32 SndTbl[] = {
//	SEQ_BGM_E_DENDOUIRI,

	SEQ_SE_DDEMO_02A,
	SEQ_SE_DDEMO_02B,
	SEQ_SE_ROTATION_B,
	SEQ_SE_DDEMO_01,
	SEQ_SE_DDEMO_03,
	SEQ_SE_DDEMO_04,
};

void DDEMOMAIN_InitSound( DDEMOMAIN_WORK * wk )
{
//	PMSND_PlayBGM( SEQ_BGM_E_DENDOUIRI );
//	PMSND_PauseBGM( TRUE );

	wk->sndHandle = SOUNDMAN_PresetSoundTbl( SndTbl, NELEMS(SndTbl) );

//	PMSND_PauseBGM( FALSE );
	PMSND_PlayBGM( SEQ_BGM_E_DENDOUIRI );
}

void DDEMOMAIN_ExitSound( DDEMOMAIN_WORK * wk )
{
	PMSND_StopBGM();
	SOUNDMAN_ReleasePresetData( wk->sndHandle );
}



void DDEMOMAIN_GetPokeMax( DDEMOMAIN_WORK * wk )
{
	wk->pokeMax = PokeParty_GetPokeCount( wk->dat->party );
}

void DDEMOMAIN_GetPokeData( DDEMOMAIN_WORK * wk )
{
	POKEMON_PARAM * pp;
	BOOL	fast;
	
	pp = PokeParty_GetMemberPointer( wk->dat->party, wk->pokePos );

	fast = PP_FastModeOn( pp );

	wk->monsno = PP_Get( pp, ID_PARA_monsno, NULL );
	wk->type   = PP_Get( pp, ID_PARA_type1, NULL );
	wk->form   = PP_Get( pp, ID_PARA_form_no, NULL );

	PP_FastModeOff( pp, fast );
}

void DDEMOMAIN_LoadPokeVoice( DDEMOMAIN_WORK * wk )
{
	// @TODO �y���b�v�`�F�b�N
	if( wk->monsno == MONSNO_PERAPPU ){
#if 0
		PMV_REF	pmvRef;
    PMV_MakeRefDataMine( &pmvRef );
		wk->voicePlayer = PMVOICE_LoadOnly( wk->monsno, wk->form, 64, FALSE, 0, 0, FALSE, (u32)&pmvRef );
#else
		wk->voicePlayer = PMVOICE_LoadOnly( wk->monsno, wk->form, 64, FALSE, 0, 0, FALSE, NULL );
#endif
	}else{
		wk->voicePlayer = PMVOICE_LoadOnly( wk->monsno, wk->form, 64, FALSE, 0, 0, FALSE, NULL );
	}
}




// ���ʃJ�����ݒ�i���l�K���j
// �œ_
#define	U_CAMERA_TX_F32	( 0.0 )
#define	U_CAMERA_TY_F32	( 192.0 )
#define	U_CAMERA_TZ_F32	( 0.0 )
//#define	U_CAMERA_TX_F32	( 0.0 )
//#define	U_CAMERA_TY_F32	( 4.5 )
//#define	U_CAMERA_TZ_F32	( -6.0 )
#define	U_CAMERA_TX		( FX_F32_TO_FX32(U_CAMERA_TX_F32) )
#define	U_CAMERA_TY		( FX_F32_TO_FX32(U_CAMERA_TY_F32) )
#define	U_CAMERA_TZ		( FX_F32_TO_FX32(U_CAMERA_TZ_F32) )
// �ʒu
#define	U_CAMERA_PX_F32	( 0.0 )
#define	U_CAMERA_PY_F32	( 18.0 )
#define	U_CAMERA_PZ_F32	( 5.6 )
#define	U_CAMERA_PX		( FX_F32_TO_FX32(U_CAMERA_PX_F32) )
#define	U_CAMERA_PY		( FX_F32_TO_FX32(U_CAMERA_PY_F32) )
#define	U_CAMERA_PZ		( FX_F32_TO_FX32(U_CAMERA_PZ_F32) )

// ����ʃJ�����ݒ�i���l�K���j
// �œ_
//#define	D_CAMERA_TX_F32	( 0.0 )
//#define	D_CAMERA_TY_F32	( 4.5 )
//#define	D_CAMERA_TZ_F32	( 2.3 )
#define	D_CAMERA_TX_F32	( 0.0 )
#define	D_CAMERA_TY_F32	( 0.0 )
#define	D_CAMERA_TZ_F32	( 0.0 )
#define	D_CAMERA_TX		( FX_F32_TO_FX32(D_CAMERA_TX_F32) )
#define	D_CAMERA_TY		( FX_F32_TO_FX32(D_CAMERA_TY_F32) )
#define	D_CAMERA_TZ		( FX_F32_TO_FX32(D_CAMERA_TZ_F32) )
// �ʒu
//#define	D_CAMERA_PX_F32	( 0.0 )
//#define	D_CAMERA_PY_F32	( 18.0 )
//#define	D_CAMERA_PZ_F32	( 14.0 )
#define	D_CAMERA_PX_F32	( 0.0 )
#define	D_CAMERA_PY_F32	( 0.0 )
#define	D_CAMERA_PZ_F32	( 0.0 )
#define	D_CAMERA_PX		( FX_F32_TO_FX32(D_CAMERA_PX_F32) )
#define	D_CAMERA_PY		( FX_F32_TO_FX32(D_CAMERA_PY_F32) )
#define	D_CAMERA_PZ		( FX_F32_TO_FX32(D_CAMERA_PZ_F32) )

#define	CAMERA_FOVY		( 18.5 )			// �c(y)�����̎��E�p�x
#define	CAMERA_ASPECT	( FX32_ONE * 1.5 )	// �c�ɑ΂��鉡�̎��E�̊���

// ���C�g�ݒ�i���l�K���j
//#define	LIGHT_PX			( FX_F32_TO_FX16(0.0) )
//#define	LIGHT_PY			( FX_F32_TO_FX16(-0.8) )
//#define	LIGHT_PZ			( FX_F32_TO_FX16(-0.5) )
#define	LIGHT_PX			( FX_F32_TO_FX16(0.0) )
#define	LIGHT_PY			( FX_F32_TO_FX16(0.0) )
#define	LIGHT_PZ			( FX_F32_TO_FX16(0.0) )
#define	LIGHT_COLOR		( 0x7fff )

// ��Βl�v�Z
/*
#define	GAME_START_CAMERA_MX_U		( U_CAMERA_PX/16 )
#define	GAME_START_CAMERA_MY_U		( U_CAMERA_PY/16 )
#define	GAME_START_CAMERA_MZ_U		( U_CAMERA_PZ/16 )
#define	GAME_START_CAMERA_MX_D		( (f32)(D_CAMERA_PX_F32-D_CAMERA_TX_F32)/(f32)16 )
#define	GAME_START_CAMERA_MY_D		( (f32)(D_CAMERA_PY_F32-D_CAMERA_TY_F32)/(f32)16 )
#define	GAME_START_CAMERA_MZ_D		( (f32)(D_CAMERA_PZ_F32-D_CAMERA_TZ_F32)/(f32)16 )

#define	GAME_START_CAMERA_MOVE_UX(cnt)	( GAME_START_CAMERA_MX_U * (16-cnt) )
#define	GAME_START_CAMERA_MOVE_UY(cnt)	( GAME_START_CAMERA_MY_U * (16-cnt) )
#define	GAME_START_CAMERA_MOVE_UZ(cnt)	( GAME_START_CAMERA_MZ_U * (16-cnt) )
#define	GAME_START_CAMERA_MOVE_DX(cnt)	( FX_F32_TO_FX32( (f32)GAME_START_CAMERA_MX_D * (f32)(16-cnt) ) )
#define	GAME_START_CAMERA_MOVE_DY(cnt)	( FX_F32_TO_FX32( (f32)GAME_START_CAMERA_MY_D * (f32)(16-cnt) ) )
#define	GAME_START_CAMERA_MOVE_DZ(cnt)	( FX_F32_TO_FX32( (f32)GAME_START_CAMERA_MZ_D * (f32)(16-cnt) ) )
*/

// �J���������_
static const VecFx32 U_ComeraTarget = { U_CAMERA_TX, U_CAMERA_TY, U_CAMERA_TZ };
static const VecFx32 D_ComeraTarget = { D_CAMERA_TX, D_CAMERA_TY, D_CAMERA_TZ };

// �J�����ʒu
static const VecFx32 U_ComeraPos = { U_CAMERA_PX, U_CAMERA_PY, U_CAMERA_PZ };
static const VecFx32 D_ComeraPos = { D_CAMERA_PX, D_CAMERA_PY, D_CAMERA_PZ };

// ���C�g�f�[�^
static const GFL_G3D_LIGHT_DATA LightData = { 0, { { LIGHT_PX, LIGHT_PY, LIGHT_PZ }, LIGHT_COLOR } };
static const GFL_G3D_LIGHTSET_SETUP LightSetup = { &LightData, 1 };

static void CreateParticleCamera( DDEMOMAIN_WORK * wk, BOOL disp );


void DDEMOMAIN_Init3D( DDEMOMAIN_WORK * wk )
{
	// �R�c�V�X�e���N��
	GFL_G3D_Init(
		GFL_G3D_VMANLNK,		// �e�N�X�`���}�l�[�W���g�p���[�h�i�t���[�����[�h�i�Ƃ���ςȂ��j�j
		GFL_G3D_TEX128K,		// �e�N�X�`���}�l�[�W���T�C�Y 128KB(1�u���b�N)
		GFL_G3D_VMANLNK,		// �p���b�g�}�l�[�W���g�p���[�h�i�t���[�����[�h�i�Ƃ���ςȂ��j�j
		GFL_G3D_PLT16K,			// �p���b�g�}�l�[�W���T�C�Y
		DTCM_SIZE,					// �W�I���g���o�b�t�@�̎g�p�T�C�Y
		HEAPID_DENDOU_DEMO,	// �q�[�vID
		NULL );							// �Z�b�g�A�b�v�֐�(NULL�̎���DefaultSetUp)

	// �n���h���쐬
	wk->g3d_util  = GFL_G3D_UTIL_Create( 0, 0, HEAPID_DENDOU_DEMO );
	// �Ǘ��V�X�e���쐬
	wk->g3d_scene = GFL_G3D_SCENE_Create(
										wk->g3d_util,						// �ˑ�����g3Dutil
										1000,										// �z�u�\�ȃI�u�W�F�N�g��
										4,											// �P�I�u�W�F�N�g�Ɋ��蓖�Ă郏�[�N�̃T�C�Y
										32,											// �A�N�Z�T����
										TRUE,										// �p�[�e�B�N���V�X�e���̋N���t���O
										HEAPID_DENDOU_DEMO );		// �q�[�vID

/*
	{	// �J�����쐬
		VecFx32 defaultCameraUp = { 0, FX32_ONE, 0 };

		wk->g3d_camera[0] = GFL_G3D_CAMERA_Create(
													GFL_G3D_PRJPERS, 
													FX_SinIdx( CAMERA_FOVY/2 *PERSPWAY_COEFFICIENT ),
													FX_CosIdx( CAMERA_FOVY/2 *PERSPWAY_COEFFICIENT ),
													CAMERA_ASPECT,
													0,
													defaultCameraNear,
													FX32_ONE,
													0,
													&D_ComeraPos,
													&defaultCameraUp,
													&D_ComeraTarget,
													HEAPID_DENDOU_DEMO );

		wk->g3d_camera[1] = GFL_G3D_CAMERA_Create(
													GFL_G3D_PRJPERS, 
													FX_SinIdx( CAMERA_FOVY/2 *PERSPWAY_COEFFICIENT ),
													FX_CosIdx( CAMERA_FOVY/2 *PERSPWAY_COEFFICIENT ),
													CAMERA_ASPECT,
													0,
													defaultCameraNear,
													FX32_ONE,
													0,
													&U_ComeraPos,
													&defaultCameraUp,
													&U_ComeraTarget,
													HEAPID_DENDOU_DEMO );
	}

	// ���C�g�쐬
	wk->g3d_light = GFL_G3D_LIGHT_Create( &LightSetup, HEAPID_DENDOU_DEMO );

	// �J�����E���C�g���f
	GFL_G3D_CAMERA_Switching( wk->g3d_camera[0] );
	GFL_G3D_LIGHT_Switching( wk->g3d_light );
*/
//	G3X_AntiAlias( TRUE );	// �Z�b�g�A�b�v�֐��ō�����ق����������ǁB�B�B

	GFL_BG_SetBGControl3D( 3 );				// BG�ʐݒ�i�����͗D��x�j
}

/*
void TITLE3D_Load( TITLE_WORK * wk )
{
	// OBJ�ǉ�
	wk->local->g3d_obj_id = GFL_G3D_SCENEOBJ_Add( wk->local->g3d_scene, ObjData, NELEMS(ObjData) );
	TITLE3D_JewelBoxAnmChange( wk, JEWELBOX_ANM_STOP );


	ParticleCreate( wk );

}
*/

void DDEMOMAIN_Exit3D( DDEMOMAIN_WORK * wk )
{
/*
	GFL_G3D_LIGHT_Delete( wk->g3d_light );
	GFL_G3D_CAMERA_Delete( wk->g3d_camera[1] );
	GFL_G3D_CAMERA_Delete( wk->g3d_camera[0] );
*/
//	GFL_G3D_SCENEOBJ_Remove( wk->local->g3d_scene, wk->local->g3d_obj_id, NELEMS(ObjData) );

	GFL_G3D_SCENE_Delete( wk->g3d_scene );
	GFL_G3D_UTIL_Delete( wk->g3d_util );

	GFL_G3D_Exit();
}

void DDEMOMAIN_InitDouble3D( DDEMOMAIN_WORK * wk )
{
	if( wk->double3dFlag == TRUE ){ return; }

	GFL_G3D_DOUBLE3D_Init( HEAPID_DENDOU_DEMO );	// 2���3D������
	wk->double3dFlag = TRUE;
}

void DDEMOMAIN_ExitDouble3D( DDEMOMAIN_WORK * wk )
{
	if( wk->double3dFlag == FALSE ){ return; }

	wk->double3dFlag = FALSE;
	GFL_G3D_DOUBLE3D_Exit();
}


void DDEMOMAIN_Main3D( DDEMOMAIN_WORK * wk )
{
	// �Q��ʂR�c
	if( wk->double3dFlag == TRUE ){
		GFL_G3D_SCENE_Main( wk->g3d_scene );
		if( GFL_G3D_DOUBLE3D_GetFlip() == TRUE ){
//			GFL_G3D_CAMERA_Switching( wk->g3d_camera[0] );
//			GFL_G3D_SCENE_SetDrawParticleSW( wk->g3d_scene, TRUE );
			CreateParticleCamera( wk, TRUE );
		}else{
//			GFL_G3D_CAMERA_Switching( wk->g3d_camera[1] );
//			GFL_G3D_SCENE_SetDrawParticleSW( wk->g3d_scene, FALSE );
			CreateParticleCamera( wk, FALSE );
		}
		GFL_G3D_SCENE_Draw( wk->g3d_scene );  
		GFL_G3D_DOUBLE3D_SetSwapFlag();
	}else{
		GFL_G3D_SCENE_Main( wk->g3d_scene );
		GFL_G3D_SCENE_Draw( wk->g3d_scene );  
	}
}


void DDEMOMAIN_InitParticle( DDEMOMAIN_WORK * wk )
{
//	GFL_G3D_PROJECTION	projection; 
	void * res;

//	GFL_PTC_Init( HEAPID_DENDOU_DEMO );

	wk->ptc = GFL_PTC_Create( wk->ptcWork, PARTICLE_LIB_HEAP_SIZE, TRUE, HEAPID_DENDOU_DEMO );
	res = GFL_PTC_LoadArcResource( ARCID_DENDOU_DEMO_GRA, NARC_dendou_demo_gra_ddemo_spa, HEAPID_DENDOU_DEMO );
	GFL_PTC_SetResource( wk->ptc, res, TRUE, NULL );
}

void DDEMOMAIN_ExitParticle( DDEMOMAIN_WORK * wk )
{
	GFL_PTC_Delete( wk->ptc );
//	GFL_PTC_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * �p�[�e�B�N���\��
 *
 * @param	wk		�ʐM��ʃ��[�N
 * @param	pos		�\�����W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void TITLE3D_ParticleSet( TITLE_WORK * wk, const VecFx32 * pos )
{
	GFL_PTC_CreateEmitter( wk->local->ptc, TITLE_PTC_TITLE_KEMU, pos );
}
*/

static void CreateParticleCamera( DDEMOMAIN_WORK * wk, BOOL disp )
{
	GFL_G3D_PROJECTION	projection;

	// ���C����ʁi���j
	if( disp == TRUE ){
		projection.param1 = FX32_CONST(3);
		projection.param2 = -FX32_CONST(3);
	// �T�u��ʁi��j
	}else{
		projection.param1 = FX32_CONST(9+2);
		projection.param2 = FX32_CONST(3+2);
	}
	projection.type = GFL_G3D_PRJORTH;
	projection.param3 = -FX32_CONST(4);
	projection.param4 = FX32_CONST(4);
	projection.near = 0;
	projection.far  = FX32_CONST( 1024 );	// ���ˉe�Ȃ̂Ŋ֌W�Ȃ����A�O�̂��߃N���b�v��far��ݒ�
	projection.scaleW = 0;

	GFL_PTC_PersonalCameraDelete( wk->ptc );
	GFL_PTC_PersonalCameraCreate( wk->ptc, &projection, DEFAULT_PERSP_WAY, NULL, NULL, NULL, HEAPID_DENDOU_DEMO );
}

void DDEMOMAIN_SetEffectParticle( DDEMOMAIN_WORK * wk, u32 id )
{
	VecFx32	p = { 0, 0, 0 };
	GFL_PTC_CreateEmitter( wk->ptc, id, &p );
}
