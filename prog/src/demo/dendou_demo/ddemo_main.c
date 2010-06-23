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
#include "system/mcss_tool.h"
#include "sound/pm_sndsys.h"
#include "sound/pm_wb_voice.h"
#include "font/font.naix"

#include "ddemo_main.h"
#include "name_spa.h"
#include "dendou_demo_gra.naix"


//============================================================================================
//	�萔��`
//============================================================================================
#define	EXP_BUF_SIZE	( 1024 )	// �e���|�������b�Z�[�W�o�b�t�@�T�C�Y

#define DTCM_SIZE		(0x1000)		// �W�I���g���o�b�t�@�̎g�p�T�C�Y


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void Scene1_VBlankTask( GFL_TCB * tcb, void * work );
static void Scene2_VBlankTask( GFL_TCB * tcb, void * work );

static void CreateParticleCamera( DDEMOMAIN_WORK * wk, GFL_PTC_PTR ptc, BOOL disp );


//============================================================================================
//	�O���[�o��
//============================================================================================

// VRAM����U��
static const GFL_DISP_VRAM VramTblScene1 = {
	GX_VRAM_BG_32_FG,							// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,				// ���C��2D�G���W����BG�g���p���b�g

	GX_VRAM_SUB_BG_NONE,					// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

	GX_VRAM_OBJ_128_B,						// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g

	GX_VRAM_SUB_OBJ_NONE,					// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g

	GX_VRAM_TEX_0_A,							// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_0123_E,				// �e�N�X�`���p���b�g�X���b�g

	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_128K		// �T�uOBJ�}�b�s���O���[�h
};

static const GFL_DISP_VRAM VramTblScene2 = {
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
	GX_OBJVRAMMODE_CHAR_1D_32K		// �T�uOBJ�}�b�s���O���[�h
};


// �^�C�v�ʃp�[�e�B�N���e�[�u��
static const u32 TypeArcTbl[] = {
	NARC_dendou_demo_gra_normal_spa,		// �m�[�}��
	NARC_dendou_demo_gra_fight_spa,			// �����Ƃ�
	NARC_dendou_demo_gra_fly_spa,				// �Ђ���
	NARC_dendou_demo_gra_poison_spa,		// �ǂ�
	NARC_dendou_demo_gra_ground_spa,		// ���߂�
	NARC_dendou_demo_gra_rock_spa,			// ����
	NARC_dendou_demo_gra_bug_spa,				// �ނ�
	NARC_dendou_demo_gra_ghost_spa,			// �S�[�X�g
	NARC_dendou_demo_gra_steel_spa,			// �͂���
	NARC_dendou_demo_gra_fire_spa,			// �ق̂�
	NARC_dendou_demo_gra_warter_spa,		// �݂�
	NARC_dendou_demo_gra_grass_spa,			// ����
	NARC_dendou_demo_gra_elec_spa,			// �ł�
	NARC_dendou_demo_gra_psyc_spa,			// �G�X�p�[
	NARC_dendou_demo_gra_ice_spa,				// ������
	NARC_dendou_demo_gra_dragon_spa,		// �h���S��
	NARC_dendou_demo_gra_dark_spa,			// ����
};

// �v���Z�b�g�r�d�e�[�u��
static const u32 SndTbl[] = {
	SEQ_SE_ROTATION_B,
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��ݒ�i�V�[���P�j
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_InitScene1VBlank( DDEMOMAIN_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( Scene1_VBlankTask, wk, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�֐��ݒ�i�V�[���Q�j
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_InitScene2VBlank( DDEMOMAIN_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( Scene2_VBlankTask, wk, 0 );
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
 * @brief		VBLANK�����i�V�[���P�j
 *
 * @param		tcb		GFL_TCB
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Scene1_VBlankTask( GFL_TCB * tcb, void * work )
{
//	GFL_BG_VBlankFunc();
	GFL_CLACT_SYS_VBlankFunc();
	GFL_G3D_DOUBLE3D_VblankIntr();
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK�����i�V�[���Q�j
 *
 * @param		tcb		GFL_TCB
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Scene2_VBlankTask( GFL_TCB * tcb, void * work )
{
	GFL_BG_VBlankFunc();
	GFL_CLACT_SYS_VBlankFunc();
//	GFL_G3D_DOUBLE3D_VblankIntr();
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VRAM������
 *
 * @param		scene		�V�[��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_InitVram( u32 scene )
{
	GFL_DISP_ClearVRAM( 0 );
	if( scene == 0 ){
		GFL_DISP_SetBank( &VramTblScene1 );
	}else{
		GFL_DISP_SetBank( &VramTblScene2 );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VRAM�ݒ�擾
 *
 * @param		scene		�V�[��
 *
 * @return	VRAM�ݒ�
 */
//--------------------------------------------------------------------------------------------
const GFL_DISP_VRAM * DDEMOMAIN_GetVramBankData( u32 scene )
{
	if( scene == 0 ){
		return &VramTblScene1;
	}
	return &VramTblScene2;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG������
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_InitBg(void)
{
	GFL_BG_Init( HEAPID_DENDOU_DEMO );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG�폜
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_ExitBg(void)
{
	GFL_BG_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG���[�h������
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_InitBgMode(void)
{
	GFL_BG_SYS_HEADER sysh = {
		GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BGMODE_3, GX_BG0_AS_3D,
	};
	GFL_BG_SetBGMode( &sysh );

	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0 );
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG�t���[���ݒ�i�V�[���Q�j
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_InitScene2BgFrame(void)
{
	{
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x08000, 0x10000,
			GX_BG_EXTPLTT_23, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &cnth, GFL_BG_MODE_256X16 );
		GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &cnth, GFL_BG_MODE_256X16 );
	}

	{	// ��]�g�k��������
		MtxFx22 mtx;
	  GFL_CALC2D_GetAffineMtx22( &mtx, 0, FX32_ONE, FX32_ONE, GFL_CALC2D_AFFINE_MAX_NORMAL );
		G2_SetBG3Affine( &mtx, 0, 0, 0, 0 );
		G2S_SetBG3Affine( &mtx, 0, 0, 0, 0 );
	}

	GFL_BG_SetScrollReq( GFL_BG_FRAME3_M, GFL_BG_SCROLL_X_SET, 160 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME3_M, GFL_BG_SCROLL_Y_SET, 112 );

	GFL_BG_SetScrollReq( GFL_BG_FRAME3_S, GFL_BG_SCROLL_X_SET, 160 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME3_S, GFL_BG_SCROLL_Y_SET, -152 );

	GFL_BG_SetRotateCenterReq( GFL_BG_FRAME3_M, GFL_BG_CENTER_X_SET, 160 );
	GFL_BG_SetRotateCenterReq( GFL_BG_FRAME3_M, GFL_BG_CENTER_Y_SET, 160 );
	GFL_BG_SetRotateCenterReq( GFL_BG_FRAME3_S, GFL_BG_CENTER_X_SET, 160 );
	GFL_BG_SetRotateCenterReq( GFL_BG_FRAME3_S, GFL_BG_CENTER_Y_SET, 160 );

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );

	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0 );
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG�t���[���폜�i�V�[���Q�j
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_ExitScene2BgFrame(void)
{
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );

	GFL_BG_FreeBGControl( GFL_BG_FRAME3_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG�ǂݍ��݁i�V�[���Q�j
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_LoadScene2BgGraphic(void)
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_DENDOU_DEMO_GRA, HEAPID_DENDOU_DEMO );

	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_dendou_demo_gra_bg_ball_NCLR, PALTYPE_MAIN_BG_EX, 0x6000, 0x20, HEAPID_DENDOU_DEMO );
	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_dendou_demo_gra_bg_ball_NCLR, PALTYPE_SUB_BG_EX, 0x6000, 0x20, HEAPID_DENDOU_DEMO );

	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_dendou_demo_gra_bg_ball_NCGR, GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_DENDOU_DEMO );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_dendou_demo_gra_bg_ball_NCGR, GFL_BG_FRAME3_S, 0, 0, FALSE, HEAPID_DENDOU_DEMO );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_dendou_demo_gra_bg_ball_NSCR, GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_DENDOU_DEMO );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_dendou_demo_gra_bg_ball_NSCR, GFL_BG_FRAME3_S, 0, 0, FALSE, HEAPID_DENDOU_DEMO );

	GFL_ARC_CloseDataHandle( ah );
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
void DDEMOMAIN_SetBlendAlpha(void)
{
	G2_SetBlendAlpha(
		GX_BLEND_PLANEMASK_BG0,
		GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD,
		6, 10 );
	G2S_SetBlendAlpha(
		GX_BLEND_PLANEMASK_BG0,
		GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD,
		6, 10 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A������
 *
 * @param		wk		�a������f����ʃ��[�N
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
	wk->exp  = GFL_STR_CreateBuffer( EXP_BUF_SIZE, HEAPID_DENDOU_DEMO );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  ���b�Z�[�W�֘A���
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_ExitMsg( DDEMOMAIN_WORK * wk )
{
	GFL_STR_DeleteBuffer( wk->exp );
	WORDSET_Delete( wk->wset );
	GFL_FONT_Delete( wk->nfnt );
	GFL_FONT_Delete( wk->font );
	GFL_MSG_Delete( wk->mman );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �T�E���h�����ݒ�
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_InitSound( DDEMOMAIN_WORK * wk )
{
	wk->sndHandle = SOUNDMAN_PresetSoundTbl( SndTbl, NELEMS(SndTbl) );
	PMSND_PlayBGM( SEQ_BGM_E_DENDOUIRI );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �T�E���h�폜
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_ExitSound( DDEMOMAIN_WORK * wk )
{
	SOUNDMAN_ReleasePresetData( wk->sndHandle );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �|�P�����ő吔�ݒ�
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_GetPokeMax( DDEMOMAIN_WORK * wk )
{
	POKEMON_PARAM * pp;
	BOOL	fast;
	u32	i;

	wk->pokeMax = 0;
	for( i=0; i<PokeParty_GetPokeCount(wk->dat->party); i++ ){
		pp = PokeParty_GetMemberPointer( wk->dat->party, i );
		fast = PP_FastModeOn( pp );
		if( PP_Get( pp, ID_PARA_tamago_flag, NULL ) == 0 ){
			wk->pp[wk->pokeMax] = pp;
			wk->pokeMax++;
		}
		PP_FastModeOff( pp, fast );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �|�P�����f�[�^�擾
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_GetPokeData( DDEMOMAIN_WORK * wk )
{
	POKEMON_PARAM * pp;
	BOOL	fast;
	
	pp = wk->pp[wk->pokePos];

	fast = PP_FastModeOn( pp );

	wk->monsno = PP_Get( pp, ID_PARA_monsno, NULL );
	wk->type   = PP_Get( pp, ID_PARA_type1, NULL );
	wk->form   = PP_Get( pp, ID_PARA_form_no, NULL );

	PP_FastModeOff( pp, fast );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  �����ǂݍ���
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_LoadPokeVoice( DDEMOMAIN_WORK * wk )
{
	if( wk->monsno == MONSNO_PERAPPU ){
#if 1
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


//============================================================================================
//	3D
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z�b�g�A�b�v
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetUp3D(void)
{
	// �R�c�g�p�ʂ̐ݒ�(�\�����v���C�I���e�B�[)
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	G2_SetBG0Priority(0);

	// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
	//	�t�H�O�A�G�b�W�}�[�L���O�A�g�D�[���V�F�[�h�̏ڍאݒ��
	//	�e�[�u�����쐬���Đݒ肵�Ă�������
	G3X_SetShading( GX_SHADING_HIGHLIGHT );	//�V�F�[�f�B���O
	G3X_AntiAlias( FALSE );									//	�A���`�G�C���A�X
	G3X_AlphaTest( FALSE, 0 );							//	�A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( TRUE );									//	�A���t�@�u�����h�@�I��
	G3X_EdgeMarking( FALSE );								//	�G�b�W�}�[�L���O
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );	//�t�H�O

	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog
	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);

	// ���C�g�ݒ�
	{
		static const GFL_G3D_LIGHT sc_GFL_G3D_LIGHT[] = 
		{
			{ { 0, -FX16_ONE, 0 }, GX_RGB( 16,16,16), },
			{ { 0, FX16_ONE, 0 }, GX_RGB( 16,16,16), },
			{ { 0, -FX16_ONE, 0 }, GX_RGB( 16,16,16), },
			{ { 0, -FX16_ONE, 0 }, GX_RGB( 16,16,16), },
		};
		int i;
		
		for( i=0; i<NELEMS(sc_GFL_G3D_LIGHT); i++ ){
			GFL_G3D_SetSystemLight( i, &sc_GFL_G3D_LIGHT[i] );
		}
	}

	//�����_�����O�X���b�v�o�b�t�@
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�c������
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
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
		SetUp3D );					// �Z�b�g�A�b�v�֐�(NULL�̎���DefaultSetUp)

	// �n���h���쐬
	wk->g3d_util  = GFL_G3D_UTIL_Create( 32, 32, HEAPID_DENDOU_DEMO );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�c�폜
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_Exit3D( DDEMOMAIN_WORK * wk )
{
	GFL_G3D_UTIL_Delete( wk->g3d_util );
	GFL_G3D_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�c���C�������i�ʏ�j
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_Main3D( DDEMOMAIN_WORK * wk )
{
	GFL_G3D_DRAW_Start();
	GFL_G3D_DRAW_SetLookAt();

	MCSS_Main( wk->mcss );
	MCSS_Draw( wk->mcss );

	GFL_G3D_DRAW_End();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�c���C�������i�Q��ʂR�c�p�j
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_MainDouble3D( DDEMOMAIN_WORK * wk )
{
	GFL_G3D_DRAW_Start();
	GFL_G3D_DRAW_SetLookAt();

	if( GFL_G3D_DOUBLE3D_GetFlip() == TRUE ){
		CreateParticleCamera( wk, wk->ptc, TRUE );
		CreateParticleCamera( wk, wk->ptcName, TRUE );
	}else{
		CreateParticleCamera( wk, wk->ptc, FALSE );
		CreateParticleCamera( wk, wk->ptcName, FALSE );
	}
	GFL_PTC_Main();

	GFL_G3D_DRAW_End();
	GFL_G3D_DOUBLE3D_SetSwapFlag();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Q��ʂR�c������
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_InitDouble3D(void)
{
	GFL_G3D_DOUBLE3D_Init( HEAPID_DENDOU_DEMO );	// 2���3D������
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Q��ʂR�c�폜
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_ExitDouble3D(void)
{
	GFL_G3D_DOUBLE3D_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p�[�e�B�N��������
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_InitParticle(void)
{
	GFL_PTC_Init( HEAPID_DENDOU_DEMO );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p�[�e�B�N���폜
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_ExitParticle(void)
{
	GFL_PTC_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�C�v�ʃp�[�e�B�N���쐬
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_CreateTypeParticle( DDEMOMAIN_WORK * wk )
{
	void * res;

	wk->ptc = GFL_PTC_Create( wk->ptcWork, PARTICLE_LIB_HEAP_SIZE, TRUE, HEAPID_DENDOU_DEMO );
	res = GFL_PTC_LoadArcResource( ARCID_DENDOU_DEMO_GRA, TypeArcTbl[wk->type], HEAPID_DENDOU_DEMO );
	GFL_PTC_SetResource( wk->ptc, res, TRUE, NULL );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�C�v�ʃp�[�e�B�N���폜
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_DeleteTypeParticle( DDEMOMAIN_WORK * wk )
{
	GFL_PTC_Delete( wk->ptc );
	wk->ptc = NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�C�v�ʃp�[�e�B�N���ǉ�
 *
 * @param		wk		�a������f����ʃ��[�N
 * @param		id		�p�[�e�B�N���h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_SetTypeParticle( DDEMOMAIN_WORK * wk, u32 id )
{
	VecFx32	p = { 0, 0, FX32_CONST(0.1f) };
	GFL_PTC_CreateEmitter( wk->ptc, id, &p );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���O�G�t�F�N�g�p�[�e�B�N���쐬
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_CreateNameParticle( DDEMOMAIN_WORK * wk )
{
	void * res;

	wk->ptcName = GFL_PTC_Create( wk->ptcNameWork, PARTICLE_LIB_HEAP_SIZE, TRUE, HEAPID_DENDOU_DEMO );
	res = GFL_PTC_LoadArcResource( ARCID_DENDOU_DEMO_GRA, NARC_dendou_demo_gra_name_spa, HEAPID_DENDOU_DEMO );
	GFL_PTC_SetResource( wk->ptcName, res, TRUE, NULL );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���O�G�t�F�N�g�p�[�e�B�N���폜
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_DeleteNameParticle( DDEMOMAIN_WORK * wk )
{
	GFL_PTC_Delete( wk->ptcName );
	wk->ptcName = NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���O�G�t�F�N�g�p�[�e�B�N���ǉ�
 *
 * @param		wk		�a������f����ʃ��[�N
 * @param		id		�p�[�e�B�N���h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_SetNameParticle( DDEMOMAIN_WORK * wk, u32 id )
{
	VecFx32	p = { 0, 0, 0 };
	GFL_PTC_CreateEmitter( wk->ptcName, id, &p );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�p�[�e�B�N���J�����쐬
 *
 * @param		wk		�a������f����ʃ��[�N
 * @param		ptc		�p�[�e�B�N��
 * @param		disp	TRUE = ���C�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
#define	PARTICLE_CAMERA_PV		( 4.0 )
#define	PARTICLE_CAMERA_PH		( 5.33 )

static void CreateParticleCamera( DDEMOMAIN_WORK * wk, GFL_PTC_PTR ptc, BOOL disp )
{
	GFL_G3D_PROJECTION	projection;

	if( ptc == NULL ){ return; }
/*
	// ���C����ʁi���j
	if( disp == TRUE ){
		projection.param1 = FX32_CONST(3.975);
		projection.param2 = -FX32_CONST(3.975);
	// �T�u��ʁi��j
	}else{
		projection.param1 = FX32_CONST(11.925+2);
		projection.param2 = FX32_CONST(3.975+2);
	}
	projection.type = GFL_G3D_PRJORTH;
	projection.param3 = -FX32_CONST(5.3);
	projection.param4 = FX32_CONST(5.3);
*/
	// ���C����ʁi���j
	if( disp == TRUE ){
		projection.param1 = FX32_CONST(PARTICLE_CAMERA_PV);
		projection.param2 = -FX32_CONST(PARTICLE_CAMERA_PV);
	// �T�u��ʁi��j
	}else{
		projection.param1 = FX32_CONST(PARTICLE_CAMERA_PV+PARTICLE_CAMERA_PV*2+2);
		projection.param2 = FX32_CONST(PARTICLE_CAMERA_PV+2);
	}
	projection.type = GFL_G3D_PRJORTH;
	projection.param3 = -FX32_CONST(PARTICLE_CAMERA_PH);
	projection.param4 = FX32_CONST(PARTICLE_CAMERA_PH);
	projection.near = 0;
	projection.far  = FX32_CONST( 1024 );	// ���ˉe�Ȃ̂Ŋ֌W�Ȃ����A�O�̂��߃N���b�v��far��ݒ�
	projection.scaleW = 0;

	GFL_PTC_PersonalCameraDelete( ptc );
	GFL_PTC_PersonalCameraCreate( ptc, &projection, DEFAULT_PERSP_WAY, NULL, NULL, NULL, HEAPID_DENDOU_DEMO );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		MCSS������
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_InitMcss( DDEMOMAIN_WORK * wk )
{
	wk->mcss = MCSS_Init( 1, HEAPID_DENDOU_DEMO );

	MCSS_SetTextureTransAdrs( wk->mcss, 0 );
	MCSS_SetOrthoMode( wk->mcss );

	{
    static const VecFx32 cam_pos = { FX_F32_TO_FX32(0.0f), FX_F32_TO_FX32(30.0f), FX_F32_TO_FX32(3000.0f) };
    static const VecFx32 cam_target = { FX_F32_TO_FX32(0.0f), FX_F32_TO_FX32(1.0f), FX_F32_TO_FX32(0.0f) };
    static const VecFx32 cam_up = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32(30.0f), FX_F32_TO_FX32(0.0f) };

    wk->mcssCamera = GFL_G3D_CAMERA_CreateOrtho(
											FX32_CONST(96),
											-FX32_CONST(96),
											-FX32_CONST(128),
											FX32_CONST(128),
											defaultCameraNear, defaultCameraFar, 0,
											&cam_pos, &cam_up, &cam_target,
                      HEAPID_DENDOU_DEMO );

    GFL_G3D_CAMERA_Switching( wk->mcssCamera );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		MCSS���
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_ExitMcss( DDEMOMAIN_WORK * wk )
{
	DDEMOMAIN_DelMcss( wk );
	GFL_G3D_CAMERA_Delete( wk->mcssCamera );
	MCSS_Exit( wk->mcss );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		MCSS�ǉ�
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_AddMcss( DDEMOMAIN_WORK * wk )
{
	MCSS_ADD_WORK   add;
	POKEMON_PARAM * pp;
	VecFx32 scale = { FX_F32_TO_FX32(16), FX_F32_TO_FX32(16), FX32_ONE };

	pp = wk->pp[wk->pokePos];

	MCSS_TOOL_MakeMAWPP( pp, &add, MCSS_DIR_FRONT );

	wk->mcssWork = MCSS_Add( wk->mcss, FX32_CONST(128+48), FX32_CONST(-96+48), 0, &add );
	MCSS_SetScale( wk->mcssWork, &scale );
	MCSS_SetAnmStopFlag( wk->mcssWork );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		MCSS�폜
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_DelMcss( DDEMOMAIN_WORK * wk )
{
	if( wk->mcssWork != NULL ){
		MCSS_Del( wk->mcss, wk->mcssWork );
		wk->mcssWork = NULL;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		MCSS�ړ�
 *
 * @param		wk		�a������f����ʃ��[�N
 * @param		mv		�ړ��l
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_MoveMcss( DDEMOMAIN_WORK * wk, s16 mv )
{
	VecFx32	pos;

	MCSS_GetPosition( wk->mcssWork, &pos );
	pos.x += FX32_CONST(mv);
	MCSS_SetPosition( wk->mcssWork, &pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		MCSS�R�[���o�b�N�֐�
 *
 * @param		data
 * @param		currentFrame
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void McssCallBackFrame( u32 data, fx32 currentFrame )
{
	DDEMOMAIN_WORK * wk = (DDEMOMAIN_WORK *)data;

	MCSS_SetAnmStopFlag( wk->mcssWork );
	wk->mcssAnmEndFlg = TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		MCSS�R�[���o�b�N�֐��Z�b�g
 *
 * @param		wk		�a������f����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void DDEMOMAIN_SetMcssCallBack( DDEMOMAIN_WORK * wk )
{
	wk->mcssAnmEndFlg = FALSE;
	MCSS_SetAnimCtrlCallBack( wk->mcssWork, (u32)wk, McssCallBackFrame, 0 );
}
