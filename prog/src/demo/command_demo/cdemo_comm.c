//============================================================================================
/**
 * @file		cdemo_comm.c
 * @brief		�R�}���h�f����� �R�}���h����
 * @author	Hiroyuki Nakamura
 * @date		09.05.07
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "sound/pm_sndsys.h"

#include "cdemo_main.h"
#include "cdemo_seq.h"
#include "cdemo_comm.h"

#include "op_demo.naix"


//============================================================================================
//	�萔��`
//============================================================================================
//#define	NCGR_START_IDX	( NARC_ld_arceus_ds_2_00000_lz_NCGR )	// �L�����J�n�ʒu
//#define	NSCR_START_IDX	( NARC_ld_arceus_ds_2_00000_NSCR )		// �X�N���[���J�n�ʒu
//#define	NCLR_START_IDX	( NARC_ld_arceus_ds_2_00000_NCLR )		// �p���b�g�J�n�ʒu
#define	NCGR_START_IDX	( 0 )	// �L�����J�n�ʒu
#define	NSCR_START_IDX	( 0 )		// �X�N���[���J�n�ʒu
#define	NCLR_START_IDX	( 0 )		// �p���b�g�J�n�ʒu

typedef int (*pCommandFunc)(CDEMO_WORK*,const int*);	// �R�}���h�֐�


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
/*
static void LoadPaletteMain( LDA_WORK * awk );
static void ObjAnmMain( LDA_WORK * awk );
static void PokeWalkObjChange( LEGEND_DEMO_WORK * wk );
*/

static int Comm_Wait( CDEMO_WORK * wk, const int * comm );
static int Comm_BlackIn( CDEMO_WORK * wk, const int * comm );
static int Comm_BlackOut( CDEMO_WORK * wk, const int * comm );
static int Comm_WhiteIn( CDEMO_WORK * wk, const int * comm );
static int Comm_WhiteOut( CDEMO_WORK * wk, const int * comm );
static int Comm_FadeMain( CDEMO_WORK * wk, const int * comm );
static int Comm_BgLoad( CDEMO_WORK * wk, const int * comm );
static int Comm_BgVanish( CDEMO_WORK * wk, const int * comm );
static int Comm_BgPriChg( CDEMO_WORK * wk, const int * comm );
static int Comm_AlphaInit( CDEMO_WORK * wk, const int * comm );
static int Comm_AlphaStart( CDEMO_WORK * wk, const int * comm );
static int Comm_AlphaNone( CDEMO_WORK * wk, const int * comm );
static int Comm_BGMSave( CDEMO_WORK * wk, const int * comm );
static int Comm_BGMStop( CDEMO_WORK * wk, const int * comm );
static int Comm_BGMPlay( CDEMO_WORK * wk, const int * comm );
static int Comm_ObjVanish( CDEMO_WORK * wk, const int * comm );
static int Comm_ObjPut( CDEMO_WORK * wk, const int * comm );
static int Comm_ObjPriChange( CDEMO_WORK * wk, const int * comm );
static int Comm_ObjBgPriChange( CDEMO_WORK * wk, const int * comm );
static int Comm_BgScreenAnime( CDEMO_WORK * wk, const int * comm );

static int Comm_DebugPrint( CDEMO_WORK * wk, const int * comm );


//============================================================================================
//	�O���[�o��
//============================================================================================

// �R�}���h�֐��e�[�u��
static const pCommandFunc CommFunc[] = {
	Comm_Wait,
	Comm_BlackIn,
	Comm_BlackOut,
	Comm_WhiteIn,
	Comm_WhiteOut,
	Comm_FadeMain,

	Comm_BgLoad,
	Comm_BgVanish,
	Comm_BgPriChg,
	Comm_AlphaInit,
	Comm_AlphaStart,
	Comm_AlphaNone,

	Comm_BGMSave,
	Comm_BGMStop,
	Comm_BGMPlay,

	Comm_ObjVanish,
	Comm_ObjPut,
	Comm_ObjBgPriChange,

	Comm_BgScreenAnime,

	Comm_DebugPrint,
};

// �R�}���h�T�C�Y
static const int CommandSize[] = {
	2,		// 00: �E�F�C�g						COMM_WAIT, wait,

	3,		// 01: �u���b�N�C��					COMM_BLACK_IN, div, sync
	3,		// 02: �u���b�N�A�E�g				COMM_BLACK_OUT, div, sync
	3,		// 03: �z���C�g�C��					COMM_WHITE_IN, div, sync
	3,		// 04: �z���C�g�A�E�g				COMM_WHITE_OUT, div, sync
	1,		// 05: �t�F�[�h���C��				COMM_FADE_MAIN,

	3,		// 06: BG�ǂݍ���					COMM_BG_LOAD, frm, graphic,
	3,		// 07: BG�\���؂�ւ�				COMM_BG_VANISH, frm, flg,
	3,		// 08: BG�v���C�I���e�B�؂�ւ�		COMM_BG_PRI_CHG, frm, pri,
	4,		// 09: �A���t�@�u�����h�ݒ�			COMM_ALPHA_INIT, plane, frm, init,
	3,		// 10: �A���t�@�u�����h�J�n			COMM_ALPHA_START, end, cnt,
	1,		// 11: �A���t�@����					COMM_ALPHA_NONE,

	1,		// 12: �Đ�����BGM���L��			COMM_BGM_SAVE,
	1,		// 13: �Đ�����BGM���~			COMM_BGM_STOP,
	2,		// 14: BGM�Đ�						COMM_BGM_PLAY, no

	2,		// 15: OBJ�\���؂�ւ�				COMM_OBJ_VANISH, flg,
	3,		// 16: OBJ���W�ݒ�					COMM_OBJ_PUT, x, y,
	2,		// 17: OBJ��BG�̃v���C�I���e�B�ݒ�	COMM_OBJ_BG_PRI_CHANGE, pri,

  2,		//  CDEMOCOMM_BG_FRM_ANIME, wait,

	2,		// 18: �f�o�b�O�v�����g				COMM_DEBUG_PRINT, id

	1,		// �I��								COMM_END,
};










int CDEMOCOMM_Main( CDEMO_WORK * wk )
{
	int	ret = CommFunc[ wk->commPos[0] ]( wk, wk->commPos );

	if( ret != CDEMOCOMM_RET_TRUE ){
		wk->commPos = &wk->commPos[ CommandSize[wk->commPos[0]] ];
	}

	return ret;
}


//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�F�E�F�C�g
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_NEXT = ���̃R�}���h��"
 * @retval	"COMM_RET_TRUE = �R�}���h���s��"
 *
 * @li	comm[0] = COMM_WAIT
 * @li	comm[1] = wait
 */
//--------------------------------------------------------------------------------------------
static int Comm_Wait( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_WAIT, "COMM_WAIT: comm[0] = %d\n", comm[0] );

	if( wk->cnt == comm[1] ){
		wk->cnt = 0;
		return CDEMOCOMM_RET_NEXT;
	}
	wk->cnt++;
	return CDEMOCOMM_RET_TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�F�u���b�N�C��
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_BLACK_IN
 * @li	comm[1] = div
 * @li	comm[2] = sync
 */
//--------------------------------------------------------------------------------------------
static int Comm_BlackIn( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_BLACK_IN, "COMM_BLACK_IN: comm[0] = %d\n", comm[0] );

	CDEMOMAIN_FadeInSet( wk, comm[1], comm[2], CDEMOSEQ_MAIN_MAIN );
	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�F�u���b�N�A�E�g
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_BLACK_OUT
 * @li	comm[1] = div
 * @li	comm[2] = sync
 */
//--------------------------------------------------------------------------------------------
static int Comm_BlackOut( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_BLACK_OUT, "COMM_BLACK_OUT: comm[0] = %d\n", comm[0] );

	CDEMOMAIN_FadeOutSet( wk, comm[1], comm[2], CDEMOSEQ_MAIN_MAIN );
	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�F�z���C�g�C��
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_WHITE_IN
 * @li	comm[1] = div
 * @li	comm[2] = sync
 */
//--------------------------------------------------------------------------------------------
static int Comm_WhiteIn( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_WHITE_IN, "COMM_WHITE_IN: comm[0] = %d\n", comm[0] );

	CDEMOMAIN_WhiteInSet( wk, comm[1], comm[2], CDEMOSEQ_MAIN_MAIN );
	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�F�z���C�g�A�E�g
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_WHITE_OUT
 * @li	comm[1] = div
 * @li	comm[2] = sync
 */
//--------------------------------------------------------------------------------------------
static int Comm_WhiteOut( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_WHITE_OUT, "COMM_WHITE_OUT: comm[0] = %d\n", comm[0] );

	CDEMOMAIN_WhiteOutSet( wk, comm[1], comm[2], CDEMOSEQ_MAIN_MAIN );
	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�F�t�F�[�h���C��
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_NEXT = ���̃R�}���h��"
 *
 * @li	comm[0] = COMM_FADE_MAIN
 */
//--------------------------------------------------------------------------------------------
static int Comm_FadeMain( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_FADE_MAIN, "COMM_FADE_MAIN: comm[0] = %d\n", comm[0] );

	wk->main_seq = CDEMOSEQ_MAIN_FADE;
	wk->next_seq = CDEMOSEQ_MAIN_MAIN;
	return CDEMOCOMM_RET_NEXT;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�FBG�ǂݍ���
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_BG_LOAD
 * @li	comm[1] = frm
 * @li	comm[2] = graphic
 */
//--------------------------------------------------------------------------------------------
static int Comm_BgLoad( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_BG_LOAD, "COMM_BG_LOAD: comm[0] = %d\n", comm[0] );

/*
	ArcUtil_HDL_BgCharSet(
		awk->ah, NCGR_START_IDX + comm[2],
		awk->bgl, comm[1], 0, 0, TRUE, HEAPID_LEGEND_DEMO );

	LoadPaletteRequest( awk, comm[1], comm[2] );

	ArcUtil_HDL_ScrnSet(
		awk->ah, NSCR_START_IDX + comm[2],
		awk->bgl, comm[1], 0, 0, TRUE, HEAPID_LEGEND_DEMO );
*/
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		wk->gra_ah, NCGR_START_IDX + comm[2], comm[1], 0, 0, TRUE, HEAPID_COMMAND_DEMO );
	if( comm[2] == 0 ){
		GFL_ARCHDL_UTIL_TransVramScreen(
			wk->gra_ah, NSCR_START_IDX + comm[2], comm[1], 0, 0, FALSE, HEAPID_COMMAND_DEMO );
	}
//	GFL_ARCHDL_UTIL_TransVramPalette(
//		wk->gra_ah, NCLR_START_IDX + comm[2], PALTYPE_MAIN_BG_EX, 0, 0, HEAPID_COMMAND_DEMO );
	CDEMOMAIN_LoadPaletteRequest( wk, comm[1], NCLR_START_IDX + comm[2] );


	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�FBG�\���؂�ւ�
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_BG_VANISH
 * @li	comm[1] = frm
 * @li	comm[2] = flg
 */
//--------------------------------------------------------------------------------------------
static int Comm_BgVanish( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_BG_VANISH, "COMM_BG_VANISH: comm[0] = %d\n", comm[0] );

	GFL_BG_SetVisible( comm[1], comm[2] );
	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�FBG�v���C�I���e�B�؂�ւ�
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_BG_PRI_CHG
 * @li	comm[1] = frm
 * @li	comm[2] = pri
 */
//--------------------------------------------------------------------------------------------
static int Comm_BgPriChg( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_BG_PRI_CHG, "COMM_BG_PRI_CHG: comm[0] = %d\n", comm[0] );

	GFL_BG_SetPriority( comm[1], comm[2] );
	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�F�A���t�@�u�����h�ݒ�
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_ALPHA_INIT
 * @li	comm[1] = plane
 * @li	comm[2] = frm
 * @li	comm[3] = init
 */
//--------------------------------------------------------------------------------------------
static int Comm_AlphaInit( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_ALPHA_INIT, "COMM_ALPHA_INIT: comm[0] = %d\n", comm[0] );

	if( comm[1] == CDEMO_PRM_BLEND_PLANE_1 ){
		wk->alpha_plane1 = comm[2];
		wk->alpha_ev1    = comm[3];
	}else{
		wk->alpha_plane2 = comm[2];
		wk->alpha_ev2    = comm[3];
	}

	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�F�A���t�@�u�����h�J�n
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_NEXT = ���̃R�}���h��"
 *
 * @li	comm[0] = COMM_ALPHA_START
 * @li	comm[1] = end
 * @li	comm[2] = cnt
 */
//--------------------------------------------------------------------------------------------
static int Comm_AlphaStart( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_ALPHA_START, "COMM_ALPHA_START: comm[0] = %d\n", comm[0] );

	wk->alpha_end_ev = comm[1];
	wk->alpha_mv_frm = comm[2];

	wk->main_seq = CDEMOSEQ_MAIN_ALPHA_BLEND;
	wk->next_seq = CDEMOSEQ_MAIN_MAIN;

	return CDEMOCOMM_RET_NEXT;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�F�A���t�@����
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_ALPHA_NONE
 */
//--------------------------------------------------------------------------------------------
static int Comm_AlphaNone( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_ALPHA_NONE, "COMM_ALPHA_NONE: comm[0] = %d\n", comm[0] );

	G2_BlendNone();

	wk->alpha_plane1 = 0;
	wk->alpha_ev1    = 0;
	wk->alpha_plane2 = 0;
	wk->alpha_ev2    = 0;

	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�FBGM�L��
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_BGM_SAVE
 */
//--------------------------------------------------------------------------------------------
static int Comm_BGMSave( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_BGM_SAVE, "COMM_BGM_SAVE: comm[0] = %d\n", comm[0] );

//	awk->bgm = Snd_NowBgmNoGet();

	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�FBGM��~
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_BGM_STOP
 */
//--------------------------------------------------------------------------------------------
static int Comm_BGMStop( CDEMO_WORK * wk, const int * comm )
{
//	u16	bgm;

	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_BGM_STOP, "COMM_BGM_STOP: comm[0] = %d\n", comm[0] );
/*
	bgm = Snd_NowBgmNoGet();
	Snd_BgmStop( bgm, 0 );
*/
	PMSND_StopBGM();

	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�FBGM�Đ�
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_BGM_PLAY
 * @li	comm[1] = no
 */
//--------------------------------------------------------------------------------------------
static int Comm_BGMPlay( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_BGM_PLAY, "COMM_BGM_PLAY: comm[0] = %d\n", comm[0] );
/*
	if( comm[1] == PRM_BGM_PLAY_SAVE ){
		Snd_BgmPlay( awk->bgm );
	}else{
		Snd_BgmPlay( comm[1] );
	}
*/
//	PMSND_PlayBGM( comm[1] );
	PMSND_PlayBGM_WideChannel( comm[1] );

	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�FOBJ�\���؂�ւ�
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_OBJ_VANISH
 * @li	comm[1] = flg
 */
//--------------------------------------------------------------------------------------------
static int Comm_ObjVanish( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_OBJ_VANISH, "COMM_OBJ_VANISH: comm[0] = %d\n", comm[0] );

//	CATS_ObjectEnableCap( awk->cap[PRM_OBJ_ARCEUS], comm[1] );

	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�FOBJ���W�ݒ�
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_OBJ_PUT
 * @li	comm[1] = x
 * @li	comm[2] = y
 */
//--------------------------------------------------------------------------------------------
static int Comm_ObjPut( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_OBJ_PUT, "COMM_OBJ_PUT: comm[0] = %d\n", comm[0] );

//	CATS_ObjectPosSetCap( awk->cap[PRM_OBJ_ARCEUS], comm[1], comm[2] );

	return CDEMOCOMM_RET_FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�FOBJ��BG�̃v���C�I���e�B�ݒ�
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_OBJ_BG_PRI_CHANGE
 * @li	comm[1] = pri
 */
//--------------------------------------------------------------------------------------------
static int Comm_ObjBgPriChange( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_OBJ_BG_PRI_CHANGE, "COMM_OBJ_BG_PRI_CHANGE: comm[0] = %d\n", comm[0] );

//	CATS_ObjectBGPriSetCap( awk->cap[PRM_OBJ_ARCEUS], comm[1] );

	return CDEMOCOMM_RET_FALSE;
}


/*
	0: �R�}���h
	1: �E�F�C�g
*/
static int Comm_BgScreenAnime( CDEMO_WORK * wk, const int * comm )
{
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_BG_FRM_ANIME, "COMM_BG_FRM_ANIME: comm[0] = %d\n", comm[0] );

//	wk->bgsa_chr  = NCGR_START_IDX + comm[1];
//	wk->bgsa_pal  = NCLR_START_IDX + comm[1];
//	wk->bgsa_scr  = NSCR_START_IDX + comm[1];
	wk->bgsa_num  = 0;
	wk->bgsa_wait = comm[1];
	wk->bgsa_cnt  = 0;
	wk->bgsa_load = 0;
	wk->bgsa_seq  = 0;

	wk->main_seq = CDEMOSEQ_MAIN_BG_SCRN_ANM;

	return CDEMOCOMM_RET_NEXT;
}


//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�F�f�o�b�O�v�����g
 *
 * @param	wk		�`���f�����[�N
 * @param	comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_DEBUG_PRINT
 */
//--------------------------------------------------------------------------------------------
static int Comm_DebugPrint( CDEMO_WORK * wk, const int * comm )
{
#ifdef PM_DEBUG
	GF_ASSERT_MSG( comm[0] == CDEMOCOMM_DEBUG_PRINT, "COMM_DEBUG_PRINT: comm[0] = %d\n", comm[0] );

	OS_Printf( "frame ID: %d = %d\n", comm[1], wk->debug_count );
	{
		RTCTime	time;
		RTC_GetTime( &time );
		OS_Printf( "sec = %d\n", time.second );
	}
	{
		if( comm[1] == 0 ){
			wk->stick = OS_GetTick();
		}else{
			wk->etick = OS_GetTick();
			OS_Printf( "tick = %d\n", OS_TicksToMilliSeconds32( wk->etick - wk->stick ) );
		}
	}
#endif	// PM_DEBUG

	return CDEMOCOMM_RET_FALSE;
}





























#if 0
#include "common.h"

#include "system/wipe.h"
#include "system/arc_util.h"
#include "system/snd_tool.h"
#include "system/clact_tool.h"
#include "system/gra_tool.h"
#include "poketool/monsno.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "field/pair_poke_info.h"

#include "../../field/pair_poke_idx.h"
#include "../../data/mmodel/mmodel.naix"

#include "ld_main.h"
#include "ld_arceus.h"
#include "ld_arceus.naix"

#include "ld_arceus.dat"


//============================================================================================
//	�萔��`
//============================================================================================


typedef int (*pCommandFunc)(LEGEND_DEMO_WORK*,const int*);	// �R�}���h�֐�

// �R�}���h�߂�l
enum {
	COMM_RET_TRUE,		// �R�}���h���s��
	COMM_RET_FALSE,		// �R�}���h�I��		( �����̃R�}���h���Đ����܂� )
	COMM_RET_NEXT,		// ���̃R�}���h��	( 1sync�҂��܂� )
};

typedef struct {
	u16	buff[256*16];
//	u16	adrs;
	u32	size;
}LDA_PALETTE;

// �A���Z�E�X�f�����[�N
typedef struct {
	GF_BGL_INI * bgl;
	ARCHANDLE * ah;

	// OBJ�֘A
	CATS_SYS_PTR	csp;
	CATS_RES_PTR	crp;
	CATS_ACT_PTR	cap[PRM_OBJ_MAX];

	const int * comm;

	u16	bgm;
	u16	dmy;

	int	alpha_plane1;
	int	alpha_ev1;
	int	alpha_plane2;
	int	alpha_ev2;
	int	alpha_end_ev;
	int	alpha_mv_frm;

	LDA_PALETTE	pltt[3];

#ifdef	PM_DEBUG
	u32	debug_count_frm;
	u32	debug_count;
#endif	// PM_DEBUG

}LDA_WORK;

//#define	NSCR_START_IDX	( NARC_ld_arceus_big10_lz_NSCR )	// �X�N���[���J�n�ʒu
//#define	NCGR_START_IDX	( NARC_ld_arceus_big10_lz_NCGR )	// �L�����J�n�ʒu
//#define	NCLR_START_IDX	( NARC_ld_arceus_big10_NCLR )		// �p���b�g�J�n�ʒu
#define	NSCR_START_IDX	( NARC_ld_arceus_ds_2_00000_lz_NSCR )	// �X�N���[���J�n�ʒu
#define	NCGR_START_IDX	( NARC_ld_arceus_ds_2_00000_lz_NCGR )	// �L�����J�n�ʒu
#define	NCLR_START_IDX	( NARC_ld_arceus_ds_2_00000_NCLR )		// �p���b�g�J�n�ʒu

#define	CLA_H_NONE	( CLACT_U_HEADER_DATA_NONE )	// �Z���A�N�^�[�w�b�_�[�f�[�^�Ȃ�

#define	CLACT_ID_COMMON		( 51433 )				// ���̉�ʂŎg�p����Z���A�N�^�[��ID

#define	SUB_ACTOR_DISTANCE	( 512*FX32_ONE )		// �T�u��ʂ�OBJ�\��Y���W�I�t�Z�b�g

// ���\�[�X�h�c
enum {
	CHR_ID_POKEWALK = CLACT_ID_COMMON,
	CHR_ID_MAX = CHR_ID_POKEWALK - CLACT_ID_COMMON + 1,

	PAL_ID_POKEWALK = CLACT_ID_COMMON,
	PAL_ID_MAX = PAL_ID_POKEWALK - CLACT_ID_COMMON + 1,

	CEL_ID_POKEWALK = CLACT_ID_COMMON,
	CEL_ID_MAX = CEL_ID_POKEWALK - CLACT_ID_COMMON + 1,

	ANM_ID_POKEWALK = CLACT_ID_COMMON,
	ANM_ID_MAX = ANM_ID_POKEWALK - CLACT_ID_COMMON + 1,
};

#define	POKEWALK_PAL_SIZE	( 0x20 )		// �A������|�P�����̃p���b�g�T�C�Y



//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void LoadPaletteMain( LDA_WORK * awk );
static void ObjAnmMain( LDA_WORK * awk );
static void PokeWalkObjChange( LEGEND_DEMO_WORK * wk );

static int MainSeq_Init( LEGEND_DEMO_WORK * wk );
static int MainSeq_Release( LEGEND_DEMO_WORK * wk );
static int MainSeq_Fade( LEGEND_DEMO_WORK * wk );
static int MainSeq_AlphaBlend( LEGEND_DEMO_WORK * wk );
static int MainSeq_Main( LEGEND_DEMO_WORK * wk );

static int Comm_Wait( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_BlackIn( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_BlackOut( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_WhiteIn( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_WhiteOut( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_FadeMain( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_BgLoad( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_BgVanish( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_BgPriChg( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_AlphaInit( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_AlphaStart( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_AlphaNone( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_BGMSave( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_BGMStop( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_BGMPlay( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_ObjVanish( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_ObjPut( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_ObjPriChange( LEGEND_DEMO_WORK * wk, const int * comm );
static int Comm_ObjBgPriChange( LEGEND_DEMO_WORK * wk, const int * comm );

static int Comm_DebugPrint( LEGEND_DEMO_WORK * wk, const int * comm );


//============================================================================================
//	�O���[�o���ϐ�
//============================================================================================

// VRAM����U��
static const GF_BGL_DISPVRAM VramTbl = {
	GX_VRAM_BG_256_BC,				// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_0123_E,		// ���C��2D�G���W����BG�g���p���b�g

	GX_VRAM_SUB_BG_32_H,			// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

	GX_VRAM_OBJ_128_A,				// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g

	GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g

	GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_NONE			// �e�N�X�`���p���b�g�X���b�g
};



// �n�a�i�f�[�^
static const TCATS_OBJECT_ADD_PARAM_S ClaPrm = {
	0, 0, 0,
	0, 0, 0, NNS_G2D_VRAM_TYPE_2DMAIN,
	{ CHR_ID_POKEWALK, PAL_ID_POKEWALK, CEL_ID_POKEWALK, ANM_ID_POKEWALK, CLA_H_NONE, CLA_H_NONE },
	0, 0
};




//--------------------------------------------------------------------------------------------
/**
 * VBLANK�֐�
 *
 * @param	work
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( void * work )
{
	LDA_WORK * awk = work;

#ifdef	PM_DEBUG
//	awk->debug_count_frm++;
//	if( awk->debug_count_frm == 60 ){
//		awk->debug_count_frm = 0;
		awk->debug_count++;
//		OS_Printf( "%d sec\n", awk->debug_count );
//	}
#endif	// PM_DEBUG

	LoadPaletteMain( awk );

	GF_BGL_VBlankFunc( awk->bgl );

	CATS_RenderOamTrans();

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------------------------------------
/**
 * VRAM�ݒ�
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VramBankSet(void)
{
	GF_Disp_SetBank( &VramTbl );
}

//--------------------------------------------------------------------------------------------
/**
 * �a�f������
 *
 * @param	awk		�A���Z�E�X�f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgInit( LDA_WORK * awk )
{
	awk->bgl = GF_BGL_BglIniAlloc( HEAPID_LEGEND_DEMO );

	{	// BG SYSTEM
		GF_BGL_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GF_BGL_InitBG( &sysh );
	}

	{	// ���C����ʁF
		GF_BGL_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( awk->bgl, GF_BGL_FRAME0_M, &cnth, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( awk->bgl, GF_BGL_FRAME0_M );
		GF_BGL_ClearCharSet( GF_BGL_FRAME0_M, 0x40, 0, HEAPID_LEGEND_DEMO );
	}
	{	// ���C����ʁF
		GF_BGL_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x20000, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( awk->bgl, GF_BGL_FRAME1_M, &cnth, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( awk->bgl, GF_BGL_FRAME1_M );
		GF_BGL_ClearCharSet( GF_BGL_FRAME1_M, 0x40, 0, HEAPID_LEGEND_DEMO );
	}
	{	// ���C����ʁF
		GF_BGL_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x30000, GX_BG_EXTPLTT_23,
			2, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( awk->bgl, GF_BGL_FRAME2_M, &cnth, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( awk->bgl, GF_BGL_FRAME2_M );
		GF_BGL_ClearCharSet( GF_BGL_FRAME2_M, 0x40, 0, HEAPID_LEGEND_DEMO );
	}

	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_M, 0 );
	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_S, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * �a�f���
 *
 * @param	awk		�A���Z�E�X�f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( LDA_WORK * awk )
{
	GF_BGL_BGControlExit( awk->bgl, GF_BGL_FRAME2_M );
	GF_BGL_BGControlExit( awk->bgl, GF_BGL_FRAME1_M );
	GF_BGL_BGControlExit( awk->bgl, GF_BGL_FRAME0_M );

	sys_FreeMemoryEz( awk->bgl );
}

//--------------------------------------------------------------------------------------------
/**
 * �n�a�i������
 *
 * @param	wk		�`���f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ObjInit( LEGEND_DEMO_WORK * wk )
{
/*
	LDA_WORK * awk = wk->work;

	awk->csp = CATS_AllocMemory( HEAPID_LEGEND_DEMO );
	awk->crp = CATS_ResourceCreate( awk->csp );

	{
		TCATS_OAM_INIT	coi = {
			0, 128,
			0, 32,
			0, 128,
			0, 32,
		};
		TCATS_CHAR_MANAGER_MAKE ccmm = {
			1024,
			1024*128,
			1024*16,
			GX_OBJVRAMMODE_CHAR_1D_32K,
			GX_OBJVRAMMODE_CHAR_1D_32K
		};
		CATS_SystemInit( awk->csp, &coi, &ccmm, 32 );
	}
	{
		TCATS_RESOURCE_NUM_LIST	crnl = {
			CHR_ID_MAX, PAL_ID_MAX, CEL_ID_MAX, ANM_ID_MAX, 0, 0
		};
		CATS_ClactSetInit( awk->csp, awk->crp, PRM_OBJ_MAX );
		CATS_ResourceManagerInit( awk->csp, awk->crp, &crnl );
	}
	{
		CLACT_U_EASYRENDER_DATA * renddata = CATS_EasyRenderGet( awk->csp );
		CLACT_U_SetSubSurfaceMatrix( renddata, 0, 1024 * FX32_ONE );	// �T�u�ɂn�a�i���o���Ȃ��̂œK��
	}

	// ���\�[�X�ǂݍ���
	CATS_LoadResourceCellArcH(
		awk->csp, awk->crp, awk->ah, NARC_ld_arceus_pw_obj_64_lz_NCER, 1, CEL_ID_POKEWALK );
	CATS_LoadResourceCellAnmArcH(
		awk->csp, awk->crp, awk->ah, NARC_ld_arceus_pw_obj_64_lz_NANR, 1, ANM_ID_POKEWALK );
	CATS_LoadResourcePlttArcH(
		awk->csp, awk->crp, awk->ah,
		NARC_ld_arceus_pw_obj_64_NCLR, 0, 1, NNS_G2D_VRAM_TYPE_2DMAIN, PAL_ID_POKEWALK );
	CATS_LoadResourceCharArcH(
		awk->csp, awk->crp, awk->ah,
		NARC_ld_arceus_pw_obj_64_lz_NCGR, 1, NNS_G2D_VRAM_TYPE_2DMAIN, CHR_ID_POKEWALK );

	awk->cap[PRM_OBJ_ARCEUS] = CATS_ObjectAdd_S( awk->csp, awk->crp, &ClaPrm );
	CATS_ObjectEnableCap( awk->cap[PRM_OBJ_ARCEUS], OBJ_OFF );

	PokeWalkObjChange( wk );

	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// MAIN DISP OBJ ON
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �n�a�i�폜
 *
 * @param	awk		�A���Z�E�X�f�����[�N
 * @param	id		OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ObjDel( LDA_WORK * awk, u32 id )
{
	if( awk->cap[id] != NULL ){
		CATS_ActorPointerDelete_S( awk->cap[id] );
		awk->cap[id] = NULL;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �n�a�i���
 *
 * @param	awk		�A���Z�E�X�f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ObjExit( LDA_WORK * awk )
{
/*
	u32	i;

	for( i=0; i<PRM_OBJ_MAX; i++ ){
		ObjDel( awk, i );
	}
	CATS_ResourceDestructor_S( awk->csp, awk->crp );
	CATS_FreeMemory( awk->csp );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �n�a�i�A�j�����C��
 *
 * @param	awk		�A���Z�E�X�f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ObjAnmMain( LDA_WORK * awk )
{
/*
	u32	i;

	for( i=0; i<PRM_OBJ_MAX; i++ ){
		if( awk->cap[i] != NULL ){
			CATS_ObjectUpdateCap( awk->cap[i] );
		}
	}
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �n�a�i�]��
 *
 * @param	buf		�]���f�[�^
 * @param	adrs	�]����A�h���X
 * @param	siz		�]���T�C�Y
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ObjVramTrans( void * buf, u32 adrs, u32 siz )
{
	DC_FlushRange( buf, siz );
	GX_LoadOBJ( buf, adrs, siz );
}

//--------------------------------------------------------------------------------------------
/**
 * �n�a�i�p���b�g�]��
 *
 * @param	buf		�]���f�[�^
 * @param	adrs	�]����A�h���X
 * @param	siz		�]���T�C�Y
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ObjPlttVramTrans( void * buf, u32 adrs, u32 siz )
{
	DC_FlushRange( buf, siz );
	GX_LoadOBJPltt( buf, adrs, siz );
}


//--------------------------------------------------------------------------------------------
/**
 * �A������|�P�����̃A�[�J�C�u�C���f�b�N�X��Ԃ�
 *
 * @param	inMonsNo	�����X�^�[�i���o�[
 * @param	inFormNo	�t�H�����i���o�[
 * @param	inSex		����
 *
 * @return	�A�[�J�C�u�C���f�b�N�X
 *
 *	pokethlon/poke_clact.c���R�s�[
 */
//--------------------------------------------------------------------------------------------
static int GetMonsArcIdx( const int inMonsNo, const u16 inFormNo, const int inSex )
{
	int idx;
	if ( (inMonsNo<=0) || (MONSNO_END<inMonsNo) ){
		idx = 1;
	}else{
		idx = NARC_mmodel_tsure_001_nsbtx + PairPokeIdx_GetIdx(inMonsNo);
		//�����X�^�[�i���o�[�������炾�ƕs�\���Ȃ̂ŁA�������炳��ɒ��ׂ�i�t�H�����Ƃ��j
		//���ʂɂ��`���Ⴂ�����āA�������t�H�����i���o�[�����|�P�����͂��Ȃ��̂ŁA
		//���ʈႢ���܂��L�[�ɂ��ĕ���
		if (PairPokeIdx_GetSexDif(inMonsNo)){		//���ʂɂ��`���Ⴂ������
			if(inSex == PARA_FEMALE ){ //���X�̏ꍇ
				idx += 1;	//�x�[�X����1�V�t�g�����Ƃ��낪���X�̃R�[�h
			}
		}else{		//���ʂɂ��`���Ⴂ���Ȃ�
			//�t�H�����𒲂ׂ�
			int form;
			if ( inFormNo > PairPokeIdx_GetFormNum(inMonsNo) ){
				form = 0;
			}else{
				form = inFormNo;
			}
			idx += form;
		}
	}

	return idx;
}

// �A������|�P�����n�a�i�T�C�Y
#define	POKEWALK_ONE_CELL_SIZE	( 0x20 * 8 * 8 )

//--------------------------------------------------------------------------------------------
/**
 * �A������|�P�����n�a�i�؂�ւ�
 *
 * @param	wk		�`���f�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeWalkObjChange( LEGEND_DEMO_WORK * wk )
{
	LDA_WORK * awk;
	u16	sex, form;
	BOOL	rare;
	NNSG3dResTex * tex;
	void * file;
	u8 * raw;

	awk = wk->work;

	{	// �|�P�����f�[�^�쐬
		POKEPARTY * party = SaveData_GetTemotiPokemon( wk->dat->savedata );

		if( PokeParty_GetPokeCount( party ) == 0 ){
			sex  = PARA_UNK;
			form = 0;
			rare = FALSE;
		}else{
			POKEMON_PARAM * pp = PokeParty_GetMemberPointer( party, 0 );

			if( PokeParaGet( pp, ID_PARA_monsno, NULL ) == MONSNO_AUSU ){
				sex  = PokeParaGet( pp, ID_PARA_sex, NULL );
				form = PokeParaGet( pp, ID_PARA_form_no, NULL);
				rare = PokeRareGet( pp );
			}else{
				sex  = PARA_UNK;
				form = 0;
				rare = FALSE;
			}
		}
	}

	{	// �A������|�P�����̃e�N�X�`����ǂݍ���ł���
		// �A�[�J�C�u�C���f�b�N�X�擾
		u32	arc = GetMonsArcIdx( MONSNO_AUSU, form, sex );

		file = ArchiveDataLoadMalloc( ARC_MMODEL, arc, HEAPID_LEGEND_DEMO );
	}

	// �e�N�X�`���t�@�C�����擾
	tex = NNS_G3dGetTex( file );

	{	// �L������������
		NNSG2dImageProxy * ipc;
		void * buf;
		u32	cgx;
		u32	i;

		ipc = CLACT_ImageProxyGet( awk->cap[PRM_OBJ_ARCEUS]->act );
		cgx = NNS_G2dGetImageLocation( ipc, NNS_G2D_VRAM_TYPE_2DMAIN );

		// �e�N�X�`�����E�f�[�^�擾
		raw = (u8 *)tex + tex->texInfo.ofsTex;

		buf = sys_AllocMemory( HEAPID_LEGEND_DEMO, POKEWALK_ONE_CELL_SIZE );

		for( i=0; i<8; i++ ){
			// �L�����f�[�^�p�̃o�b�t�@�Ƀe�N�X�`���̃��E�f�[�^��1D�ϊ����Ċi�[
			ChangesInto_RawData_1D_from_2D( &raw[POKEWALK_ONE_CELL_SIZE*i], 8, 0, 0, 8, 8, buf );
			ObjVramTrans( buf, cgx+(POKEWALK_ONE_CELL_SIZE*i), POKEWALK_ONE_CELL_SIZE );
		}

		sys_FreeMemoryEz( buf );
	}

	{	// �p���b�g��������
		NNSG2dImagePaletteProxy * ipp;
		u32	pal;

		ipp = CLACT_PaletteProxyGet( awk->cap[PRM_OBJ_ARCEUS]->act );
		pal = NNS_G2dGetImagePaletteLocation( ipp, NNS_G2D_VRAM_TYPE_2DMAIN );

		// �p���b�g���E�f�[�^�擾
		raw = (u8 *)tex + tex->plttInfo.ofsPlttData;
		// ���A���ǂ����ŃI�t�Z�b�g�ύX
		if( rare == TRUE ){
			raw = &raw[POKEWALK_PAL_SIZE];
		}
		ObjPlttVramTrans( raw, pal, POKEWALK_PAL_SIZE );
	}

	sys_FreeMemoryEz( file );
}






#endif
