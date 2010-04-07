//============================================================================================
/**
 * @file		ddemo_seq.c
 * @brief		�a������f�����
 * @author	Hiroyuki Nakamura
 * @date		10.04.06
 *
 *	���W���[�����FDDEMOSEQ
 */
//============================================================================================
#include <gflib.h>

#include "system/wipe.h"
#include "sound/pm_sndsys.h"

#include "ddemo_main.h"
#include "ddemo_seq.h"
#include "ddemo_obj.h"


static int MainSeq_Init( DDEMOMAIN_WORK * wk );
static int MainSeq_Release( DDEMOMAIN_WORK * wk );
static int MainSeq_Wipe( DDEMOMAIN_WORK * wk );
static int MainSeq_Main( DDEMOMAIN_WORK * wk );

static int SetFadeIn( DDEMOMAIN_WORK * wk, int next );
static int SetFadeOut( DDEMOMAIN_WORK * wk, int next );


static const pDDEMOMAIN_FUNC MainSeq[] = {
	MainSeq_Init,
	MainSeq_Release,
	MainSeq_Wipe,

	MainSeq_Main,
};




BOOL DDEMOSEQ_MainSeq( DDEMOMAIN_WORK * wk )
{
	wk->mainSeq = MainSeq[wk->mainSeq]( wk );
	if( wk->mainSeq == MAINSEQ_END ){
		return FALSE;
	}

	DDEMOMAIN_Main3D( wk );
	GFL_G3D_DOUBLE3D_SetSwapFlag();

	return TRUE;
}



static int MainSeq_Init( DDEMOMAIN_WORK * wk )
{
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �T�u��ʂ����C����
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );
	// �P�x���Œ�ɂ��Ă���
	GX_SetMasterBrightness( -16 );
	GXS_SetMasterBrightness( -16 );


	DDEMOMAIN_InitVram();
	DDEMOMAIN_InitBg();

	DDEMOMAIN_Init3D( wk );
	DDEMOMAIN_InitParticle( wk );

	DDEMOMAIN_SetBlendAlpha();

	DDEMOMAIN_InitVBlank( wk );

	return SetFadeIn( wk, MAINSEQ_MAIN );
}

static int MainSeq_Release( DDEMOMAIN_WORK * wk )
{
	DDEMOMAIN_ExitVBlank( wk );

	DDEMOMAIN_ExitParticle( wk );
	DDEMOMAIN_Exit3D( wk );

	DDEMOMAIN_ExitBg();

	// �P�x���Œ�ɂ��Ă���
	GX_SetMasterBrightness( -16 );
	GXS_SetMasterBrightness( -16 );
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );

	return MAINSEQ_END;
}

static int MainSeq_Wipe( DDEMOMAIN_WORK * wk )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		return wk->nextSeq;
	}
	return MAINSEQ_WIPE;
}

static int MainSeq_Main( DDEMOMAIN_WORK * wk )
{
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
		DDEMOMAIN_SetEffectParticle( wk, 1 );
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
		return SetFadeOut( wk, MAINSEQ_RELEASE );
	}
	return MAINSEQ_MAIN;
}





static int SetFadeIn( DDEMOMAIN_WORK * wk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_DENDOU_DEMO );
	wk->nextSeq = next;
	return MAINSEQ_WIPE;
}

static int SetFadeOut( DDEMOMAIN_WORK * wk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_DENDOU_DEMO );
	wk->nextSeq = next;
	return MAINSEQ_WIPE;
}
