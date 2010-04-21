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
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"
#include "sound/pm_wb_voice.h"

#include "ddemo_main.h"
#include "ddemo_seq.h"
#include "ddemo_obj.h"


static int MainSeq_Init( DDEMOMAIN_WORK * wk );
static int MainSeq_Release( DDEMOMAIN_WORK * wk );
static int MainSeq_Wipe( DDEMOMAIN_WORK * wk );
static int MainSeq_Wait( DDEMOMAIN_WORK * wk );

static int MainSeq_1stInit( DDEMOMAIN_WORK * wk );
static int MainSeq_1stMain( DDEMOMAIN_WORK * wk );
static int MainSeq_1stExit( DDEMOMAIN_WORK * wk );
static int MainSeq_2ndInit( DDEMOMAIN_WORK * wk );
static int MainSeq_2ndMain( DDEMOMAIN_WORK * wk );
static int MainSeq_2ndExit( DDEMOMAIN_WORK * wk );

static int SetFadeIn( DDEMOMAIN_WORK * wk, int next );
static int SetFadeOut( DDEMOMAIN_WORK * wk, int next );

static int SetWait( DDEMOMAIN_WORK * wk, int wait );

static void MakeRandMoveParam( DDEMOMAIN_WORK * wk );
static BOOL MoveObjRand( DDEMOMAIN_WORK * wk, u32 id );

static void RotationBgScene2( DDEMOMAIN_WORK * wk );


static const pDDEMOMAIN_FUNC MainSeq[] = {
	MainSeq_Init,
	MainSeq_Release,
	MainSeq_Wipe,
	MainSeq_Wait,

	MainSeq_1stInit,
	MainSeq_1stMain,
	MainSeq_1stExit,

	MainSeq_2ndInit,
	MainSeq_2ndMain,
	MainSeq_2ndExit,
};




BOOL DDEMOSEQ_MainSeq( DDEMOMAIN_WORK * wk )
{
	wk->mainSeq = MainSeq[wk->mainSeq]( wk );
	if( wk->mainSeq == MAINSEQ_END ){
		return FALSE;
	}

	if( wk->scene == 1 ){
		DDEMOOBJ_MainScene1( wk );
		DDEMOMAIN_MainDouble3D( wk );
//		GFL_G3D_DOUBLE3D_SetSwapFlag();
	}else if( wk->scene == 2 ){
		DDEMOOBJ_MainScene2( wk );
		DDEMOMAIN_Main3D( wk );
		RotationBgScene2( wk );
	}

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
	// �P�x���Œ�ɂ��Ă���
	GX_SetMasterBrightness( -16 );
	GXS_SetMasterBrightness( -16 );

	DDEMOMAIN_InitBg();
	DDEMOMAIN_InitMsg( wk );
	DDEMOMAIN_InitSound( wk );

	DDEMOMAIN_GetPokeMax( wk );

/*
	if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ){
		return MAINSEQ_2ND_INIT;
	}
*/

	return MAINSEQ_1ST_INIT;
}

static int MainSeq_Release( DDEMOMAIN_WORK * wk )
{
	DDEMOMAIN_ExitSound( wk );
	DDEMOMAIN_ExitMsg( wk );
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

static int MainSeq_Wait( DDEMOMAIN_WORK * wk )
{
	if( wk->wait == 0 ){
		return wk->nextSeq;
	}
	wk->wait--;
	return MAINSEQ_WAIT;
}


static int MainSeq_1stInit( DDEMOMAIN_WORK * wk )
{
	DDEMOMAIN_InitVram( 0 );
	DDEMOMAIN_InitBgMode();

	DDEMOMAIN_Init3D( wk );
	DDEMOMAIN_InitParticle();
	DDEMOMAIN_InitDouble3D();
//	DDEMOMAIN_CreateNameParticle( wk );

	DDEMOOBJ_Init( wk, 0 );
	DDEMOOBJ_InitScene1( wk );

	DDEMOMAIN_SetBlendAlpha();

	DDEMOMAIN_InitScene1VBlank( wk );

	wk->scene = 1;

	return MAINSEQ_1ST_MAIN;
}

static int MainSeq_1stExit( DDEMOMAIN_WORK * wk )
{
	DDEMOMAIN_ExitVBlank( wk );

	G2_BlendNone();
	G2S_BlendNone();

	DDEMOOBJ_ExitScene1( wk );
	DDEMOOBJ_Exit( wk );

//	DDEMOMAIN_DeleteNameParticle( wk );
	DDEMOMAIN_ExitDouble3D();
	DDEMOMAIN_ExitParticle();
	DDEMOMAIN_Exit3D( wk );

	wk->scene = 0;

	return MAINSEQ_2ND_INIT;
}


#define	DEF_1ST_START_WAIT		( 32 )
#define	DEF_1ST_PM_LEFT_SPEED	( -16 )
#define	DEF_1ST_PM_LEFT_COUNT	( (256+96) / GFL_STD_Abs(DEF_1ST_PM_LEFT_SPEED) )
#define	DEF_1ST_PM_LEFT_WAIT	( 8 )
#define	DEF_1ST_TYPE_EFF_WAIT	( 8 )

#define	DEF_1ST_PM_RIGHT_SPEED	( 16 )
#define	DEF_1ST_PM_RIGHT_EPX		( 48 )
#define	DEF_1ST_MM_RIGHT_EPX		( 21*8 )
#define	DEF_1ST_IM_RIGHT_EPX		( 11*8 )
#define	DEF_1ST_PM_RIGHT_WAIT		( 128 )

#define	DEF_1ST_PM_OUT_SPEED	( 16 )
#define	DEF_1ST_PM_OUT_EPX		( -48 )
#define	DEF_1ST_MM_OUT_EPX		( -8 )
#define	DEF_1ST_IM_OUT_EPX		( 256+8 )

#define	DEF_1ST_LOOP_WAIT		( 32 )

static int MainSeq_1stMain( DDEMOMAIN_WORK * wk )
{
/*
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
		PMSND_StopBGM();
		return SetFadeOut( wk, MAINSEQ_1ST_EXIT );
	}
*/

	switch( wk->subSeq ){
	case 0:		// �����E�F�C�g
		wk->subSeq++;
		return SetWait( wk, DEF_1ST_START_WAIT );

	case 1:
		PMSND_PlaySE( SEQ_SE_DDEMO_02A );
		wk->subSeq++;
		break;

	case 2:
		wk->wait++;
		if( wk->wait == 48 ){
//    if( PMSND_CheckPlayingSEIdx(SEQ_SE_DDEMO_02A) == FALSE ){
//			OS_Printf( "se wait = %d\n", wk->wait );
//			PMSND_StopSE();
			PMSND_PlaySE( SEQ_SE_DDEMO_02B );
			wk->wait = 0;
			wk->subSeq++;
		}
		break;

	case 3:		// �|�P�����Z�b�g
    if( PMSND_CheckPlayingSEIdx(SEQ_SE_DDEMO_02B) == FALSE ){
			PMSND_PlaySE( SEQ_SE_DDEMO_01 );
			DDEMOMAIN_GetPokeData( wk );
			DDEMOMAIN_LoadPokeVoice( wk );
			DDEMOMAIN_CreateTypeParticle( wk );
			DDEMOMAIN_CreateNameParticle( wk );
			DDEMOOBJ_AddPoke( wk );
			DDEMOOBJ_PrintPokeInfo( wk );
			DDEMOOBJ_MoveFontOamPos( wk );
			DDEMOOBJ_SetTypeWindow( wk );
			wk->subSeq++;
			return SetFadeIn( wk, MAINSEQ_1ST_MAIN );
		}
		break;

	case 4:		// �E���獶�փ|�P�����ړ�
		if( wk->wait == DEF_1ST_PM_LEFT_COUNT ){
			wk->subSeq++;
			return SetWait( wk, DEF_1ST_PM_LEFT_WAIT );
		}else{
			DDEMOOBJ_Move( wk, DDEMOOBJ_ID_POKE_B, DEF_1ST_PM_LEFT_SPEED, 0 );
			wk->wait++;
		}
		break;

	case 5:		// �^�C�v�ʃG�t�F�N�g�\��
		DDEMOMAIN_SetTypeParticle( wk, 0 );
		DDEMOMAIN_SetTypeParticle( wk, 1 );
		wk->subSeq++;
		return SetWait( wk, DEF_1ST_TYPE_EFF_WAIT );

	case 6:
		DDEMOMAIN_SetNameParticle( wk, 1 );
		wk->subSeq++;

	case 7:		// ���b�Z�[�W�ړ�
		{
			s16	x, y;
			DDEMOOBJ_GetPos( wk, DDEMOOBJ_ID_MES, &x, &y );
			if( x != DEF_1ST_MM_RIGHT_EPX ){
				DDEMOOBJ_Move( wk, DDEMOOBJ_ID_MES, DEF_1ST_PM_RIGHT_SPEED, 0 );
			}else{
				PMSND_PlaySE( SEQ_SE_ROTATION_B );
				MakeRandMoveParam( wk );
				wk->rndDx = x;
				wk->rndDy = y;
				wk->subSeq++;
			}
			DDEMOOBJ_MoveFontOamPos( wk );
		}
		break;

	case 8:
		DDEMOMAIN_SetNameParticle( wk, 0 );
		wk->subSeq++;

	case 9:		// ���ړ�
		{
			BOOL	flg;
			s16	x, y;
			flg = MoveObjRand( wk, DDEMOOBJ_ID_MES );
			DDEMOOBJ_GetPos( wk, DDEMOOBJ_ID_INFO, &x, &y );
			if( x != DEF_1ST_IM_RIGHT_EPX ){
				DDEMOOBJ_Move( wk, DDEMOOBJ_ID_INFO, -DEF_1ST_PM_RIGHT_SPEED, 0 );
			}else if( flg == FALSE ){
				PMSND_PlaySE( SEQ_SE_ROTATION_B );
				MakeRandMoveParam( wk );
				wk->rndDx = x;
				wk->rndDy = y;
				wk->subSeq++;
			}
			DDEMOOBJ_MoveFontOamPos( wk );
		}
		break;

	case 10:		// �|�P�����ړ�
		{
			BOOL	flg;
			s16	x, y;
			flg = MoveObjRand( wk, DDEMOOBJ_ID_INFO );
			DDEMOOBJ_GetPos( wk, DDEMOOBJ_ID_POKE, &x, &y );
			if( x != DEF_1ST_PM_RIGHT_EPX ){
				DDEMOOBJ_Move( wk, DDEMOOBJ_ID_POKE, DEF_1ST_PM_RIGHT_SPEED, 0 );
			}else if( flg == FALSE ){
				PMSND_PlaySE( SEQ_SE_ROTATION_B );
				MakeRandMoveParam( wk );
				wk->rndDx = x;
				wk->rndDy = y;
				wk->subSeq++;
			}
			DDEMOOBJ_MoveFontOamPos( wk );
		}
		break;

	case 11:		// �������L���L��
		if( MoveObjRand( wk, DDEMOOBJ_ID_POKE ) == FALSE ){
			PMVOICE_PlayOnly( wk->voicePlayer );
			DDEMOOBJ_SetVanish( wk, DDEMOOBJ_ID_EFF, TRUE );
			DDEMOOBJ_SetAutoAnm( wk, DDEMOOBJ_ID_EFF, 6 );
			wk->subSeq++;
			return SetWait( wk, DEF_1ST_PM_RIGHT_WAIT );
		}
		break;

	case 12:		// �������L���L���҂�
		if( PMVOICE_CheckPlay( wk->voicePlayer ) == FALSE &&
				DDEMOOBJ_CheckAnm( wk, DDEMOOBJ_ID_EFF ) == FALSE ){
			wk->subSeq++;
		}
		break;

	case 13:		// �|�P�����A�E�g	
		{
			s16	x, y;
			DDEMOOBJ_GetPos( wk, DDEMOOBJ_ID_POKE, &x, &y );
			if( x != DEF_1ST_PM_OUT_EPX ){
				DDEMOOBJ_Move( wk, DDEMOOBJ_ID_POKE, -DEF_1ST_PM_OUT_SPEED, 0 );
			}else{
				wk->subSeq++;
			}
		}
		break;

	case 14:		// ���A�E�g
		{
			s16	x, y;
			DDEMOOBJ_GetPos( wk, DDEMOOBJ_ID_INFO, &x, &y );
			if( x != DEF_1ST_IM_OUT_EPX ){
				DDEMOOBJ_Move( wk, DDEMOOBJ_ID_INFO, DEF_1ST_PM_OUT_SPEED, 0 );
			}else{
				wk->subSeq++;
			}
			DDEMOOBJ_MoveFontOamPos( wk );
		}
		break;

	case 15:		// ���b�Z�[�W�A�E�g
		{
			s16	x, y;
			DDEMOOBJ_GetPos( wk, DDEMOOBJ_ID_MES, &x, &y );
			if( x != DEF_1ST_MM_OUT_EPX ){
				DDEMOOBJ_Move( wk, DDEMOOBJ_ID_MES, -DEF_1ST_PM_OUT_SPEED, 0 );
				DDEMOOBJ_MoveFontOamPos( wk );
			}else{
				wk->subSeq++;
				return SetFadeOut( wk, MAINSEQ_1ST_MAIN );
			}
		}
		break;

	case 16:		// �I���`�F�b�N
		DDEMOMAIN_DeleteNameParticle( wk );
		DDEMOMAIN_DeleteTypeParticle( wk );
/*
		// �f�o�b�O�p�X�L�b�v����
		if( GFL_UI_KEY_GetCont() & PAD_BUTTON_A ){
			wk->pokePos = 0;
			wk->subSeq = 0;
			return MAINSEQ_1ST_EXIT;
		}
*/
		wk->pokePos++;
		if( wk->pokePos == wk->pokeMax ){
			wk->pokePos = 0;
			wk->subSeq = 0;
			return MAINSEQ_1ST_EXIT;
		}else{
			wk->subSeq = 3;
		}
		break;
	}

	return MAINSEQ_1ST_MAIN;
}




static int MainSeq_2ndInit( DDEMOMAIN_WORK * wk )
{
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );		// �T�u��ʂ����C����

	DDEMOMAIN_InitVram( 1 );
	DDEMOMAIN_InitBgMode();

	DDEMOMAIN_InitScene2BgFrame();
	DDEMOMAIN_LoadScene2BgGraphic();

	DDEMOMAIN_Init3D( wk );
	DDEMOMAIN_InitMcss( wk );

//	DDEMOMAIN_AddMcss( wk );

	DDEMOOBJ_Init( wk, 1 );
	DDEMOOBJ_InitScene2( wk );

//	DDEMOMAIN_SetBlendAlpha();

	DDEMOMAIN_InitScene2VBlank( wk );

	wk->scene = 2;

	return SetFadeIn( wk, MAINSEQ_2ND_MAIN );
}

static int MainSeq_2ndExit( DDEMOMAIN_WORK * wk )
{
	DDEMOMAIN_ExitVBlank( wk );

//	G2_BlendNone();
//	G2S_BlendNone();

	DDEMOOBJ_ExitScene2( wk );
	DDEMOOBJ_Exit( wk );

	DDEMOMAIN_ExitScene2BgFrame();

//	DDEMOMAIN_DelMcss( wk );

	DDEMOMAIN_ExitMcss( wk );
	DDEMOMAIN_Exit3D( wk );

	wk->scene = 0;

	return MAINSEQ_RELEASE;
}



#define	DEF_2ND_START_WAIT				( 32 )
#define	DEF_2ND_PLAYER_FALL_SPEED	( 8 )
#define	DEF_2ND_PLAYER_FALL_COUNT	( (192+128+24+64)/DEF_2ND_PLAYER_FALL_SPEED )
#define	DEF_2ND_WIN_OPEN_WAIT			( 32 )
#define	DEF_2ND_WIN_PUT_WAIT			( 16 )
#define	DEF_2ND_INFO_PUT_WAIT			( 32 )
#define	DEF_2ND_MES_PUT_WAIT			( 60*5 )

#define	DEF_2ND_POKEIN_WAIT				( 128 )
#define	DEF_2ND_POKEIN_WAIT_MAX		( 256 )
#define	DEF_2ND_POKEIN_SPEED			( -8 )
#define	DEF_2ND_POKEIN_COUNT			( 19 )
#define	DEF_2ND_POKEOUT_WAIT			( 128 )
#define	DEF_2ND_POKEOUT_SPEED			( -8 )
#define	DEF_2ND_POKEOUT_COUNT			( 32+12-DEF_2ND_POKEIN_COUNT )

static int MainSeq_2ndMain( DDEMOMAIN_WORK * wk )
{
	switch( wk->subSeq ){
	case 0:		// ������
		wk->subSeq++;
		return SetWait( wk, DEF_2ND_START_WAIT );

	case 1:
		PMSND_PlaySE( SEQ_SE_DDEMO_03 );
		wk->subSeq++;
		break;

	case 2:		// ��l������
		if( wk->wait != DEF_2ND_PLAYER_FALL_COUNT ){
			DDEMOOBJ_Move( wk, DDEMOOBJ_ID_PLAYER_M, 0, DEF_2ND_PLAYER_FALL_SPEED );
			DDEMOOBJ_Move( wk, DDEMOOBJ_ID_PLAYER_S, 0, DEF_2ND_PLAYER_FALL_SPEED );
			wk->wait++;
		}else{
			PMSND_PlaySE( SEQ_SE_ROTATION_B );
			wk->subSeq++;
			return SetWait( wk, DEF_2ND_WIN_OPEN_WAIT );
		}
		break;

	case 3:		// �E�B���h�E�I�[�v��
		DDEMOOBJ_SetVanish( wk, DDEMOOBJ_ID_2ND_MES, TRUE );
		DDEMOOBJ_SetVanish( wk, DDEMOOBJ_ID_2ND_INFO, TRUE );
		DDEMOOBJ_SetAutoAnm( wk, DDEMOOBJ_ID_2ND_MES, 2 );
		DDEMOOBJ_SetAutoAnm( wk, DDEMOOBJ_ID_2ND_INFO, 2 );
		wk->subSeq++;
		break;

	case 4:		// �E�B���h�E�\���҂�
		if( DDEMOOBJ_CheckAnm( wk, DDEMOOBJ_ID_2ND_MES ) == FALSE ){
			PMSND_PlaySE( SEQ_SE_DDEMO_04 );
			wk->subSeq++;
			return SetWait( wk, DEF_2ND_WIN_PUT_WAIT );
		}
		break;

	case 5:		// �v���C���[���\��
		BOX2OBJ_FontOamVanish( wk, DDEMOOBJ_FOAM_PLAYER, TRUE );
		wk->subSeq++;
		return SetWait( wk, DEF_2ND_INFO_PUT_WAIT );

	case 6:		// ���b�Z�[�W�\��
		BOX2OBJ_FontOamVanish( wk, DDEMOOBJ_FOAM_MES2, TRUE );
		wk->subSeq++;
		break;

	case 7:		// �E�F�C�g
		if( wk->wait == DEF_2ND_MES_PUT_WAIT ){
			wk->wait = 0;
			wk->subSeq++;
		}else{
			wk->wait++;
		}
		break;

	// �I���҂�
	case 8:
		{
			BOOL	flg = FALSE;
			if( wk->pokePos == 0 ){
				if( wk->wait == DEF_2ND_POKEIN_WAIT_MAX ){
					flg = TRUE;
				}
			}else{
				if( wk->wait == DEF_2ND_POKEIN_WAIT ){
					flg = TRUE;
				}
			}
			if( flg == TRUE ){
				DDEMOMAIN_AddMcss( wk );
				wk->wait = 0;
				wk->subSeq++;
				break;
			}
			wk->wait++;
		}
		break;

	case 9:
		if( wk->wait != DEF_2ND_POKEIN_COUNT ){
			DDEMOMAIN_MoveMcss( wk, DEF_2ND_POKEIN_SPEED );
			wk->wait++;
		}else{
			MCSS_ResetAnmStopFlag( wk->mcssWork );
			wk->wait = 0;
			wk->subSeq++;
		}
		break;

	case 10:
		if( wk->wait != DEF_2ND_POKEOUT_WAIT ){
			wk->wait++;
		}else{
			DDEMOMAIN_SetMcssCallBack( wk );
			wk->wait = 0;
			wk->subSeq++;
		}
		break;

	case 11:
		if( wk->mcssAnmEndFlg == FALSE ){
			break;
		}
		if( wk->wait != DEF_2ND_POKEOUT_COUNT ){
			DDEMOMAIN_MoveMcss( wk, DEF_2ND_POKEOUT_SPEED );
			wk->wait++;
		}else{
			DDEMOMAIN_DelMcss( wk );
			wk->wait = 0;
			wk->pokePos++;
			if( wk->pokePos == wk->pokeMax ){
				wk->pokePos = 0;
			}
			wk->subSeq = 8;
		}
		break;
	}

	if( wk->subSeq >= 6 ){
		DDEMOOBJ_SetRandomFlash( wk );
	}
	if( wk->subSeq >= 8 ){
		if( ( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ) ||
				GFL_UI_TP_GetTrg() == TRUE ){
			return SetFadeOut( wk, MAINSEQ_2ND_EXIT );
		}
	}

	return MAINSEQ_2ND_MAIN;
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

static int SetWait( DDEMOMAIN_WORK * wk, int wait )
{
	wk->wait = wait;
	wk->nextSeq = wk->mainSeq;
	return MAINSEQ_WAIT;
}



static void MakeRandMoveParam( DDEMOMAIN_WORK * wk )
{
	u32	i;

	wk->rndCnt = 0;

	for( i=0; i<8; i++ ){
		wk->rndMx[i] = GFUser_GetPublicRand( 8 ) - 4;
		wk->rndMy[i] = GFUser_GetPublicRand( 8 ) - 4;
	}
}

static BOOL MoveObjRand( DDEMOMAIN_WORK * wk, u32 id )
{
	if( wk->rndCnt == 8 ){
		DDEMOOBJ_SetPos( wk, id, wk->rndDx, wk->rndDy );
		return FALSE;
	}

	DDEMOOBJ_SetPos( wk, id, wk->rndDx+wk->rndMx[wk->rndCnt], wk->rndDy+wk->rndMy[wk->rndCnt] );
	wk->rndCnt++;
	return TRUE;
}

static void RotationBgScene2( DDEMOMAIN_WORK * wk )
{
	GFL_BG_SetRadianReq( GFL_BG_FRAME3_M, GFL_BG_RADION_INC, 1 );
	GFL_BG_SetRadianReq( GFL_BG_FRAME3_S, GFL_BG_RADION_INC, 1 );
}
