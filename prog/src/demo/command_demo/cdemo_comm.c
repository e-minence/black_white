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
#define	NCGR_START_IDX	( 0 )		// �L�����J�n�ʒu
#define	NSCR_START_IDX	( 0 )		// �X�N���[���J�n�ʒu
#define	NCLR_START_IDX	( 0 )		// �p���b�g�J�n�ʒu

typedef int (*pCommandFunc)(CDEMO_WORK*,const int*);	// �R�}���h�֐�


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
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
	2,		// 00: �E�F�C�g							COMM_WAIT, wait,

	3,		// 01: �u���b�N�C��					COMM_BLACK_IN, div, sync
	3,		// 02: �u���b�N�A�E�g				COMM_BLACK_OUT, div, sync
	3,		// 03: �z���C�g�C��					COMM_WHITE_IN, div, sync
	3,		// 04: �z���C�g�A�E�g				COMM_WHITE_OUT, div, sync
	1,		// 05: �t�F�[�h���C��				COMM_FADE_MAIN,

	3,		// 06: BG�ǂݍ���									COMM_BG_LOAD, frm, graphic,
	3,		// 07: BG�\���؂�ւ�							COMM_BG_VANISH, frm, flg,
	3,		// 08: BG�v���C�I���e�B�؂�ւ�		COMM_BG_PRI_CHG, frm, pri,
	4,		// 09: �A���t�@�u�����h�ݒ�				COMM_ALPHA_INIT, plane, frm, init,
	3,		// 10: �A���t�@�u�����h�J�n				COMM_ALPHA_START, end, cnt,
	1,		// 11: �A���t�@����								COMM_ALPHA_NONE,

	1,		// 12: �Đ�����BGM���L��			COMM_BGM_SAVE,
	1,		// 13: �Đ�����BGM���~			COMM_BGM_STOP,
	2,		// 14: BGM�Đ�								COMM_BGM_PLAY, no

	2,		// 15: OBJ�\���؂�ւ�							COMM_OBJ_VANISH, flg,
	3,		// 16: OBJ���W�ݒ�									COMM_OBJ_PUT, x, y,
	2,		// 17: OBJ��BG�̃v���C�I���e�B�ݒ�	COMM_OBJ_BG_PRI_CHANGE, pri,

  2,		// 18: BG�X�N���[���A�j��			CDEMOCOMM_BG_FRM_ANIME, wait,

	2,		// 19: �f�o�b�O�v�����g				COMM_DEBUG_PRINT, id

	1,		// �I��								COMM_END,
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		���C������
 *
 * @param		wk		���[�N
 *
 * @return	�������e
 */
//--------------------------------------------------------------------------------------------
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
 * @brief		�R�}���h�F�E�F�C�g
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�F�u���b�N�C��
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�F�u���b�N�A�E�g
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�F�z���C�g�C��
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�F�z���C�g�A�E�g
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�F�t�F�[�h���C��
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�FBG�ǂݍ���
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�FBG�\���؂�ւ�
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�FBG�v���C�I���e�B�؂�ւ�
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�F�A���t�@�u�����h�ݒ�
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�F�A���t�@�u�����h�J�n
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�F�A���t�@����
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�FBGM�L��
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�FBGM��~
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�FBGM�Đ�
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�FOBJ�\���؂�ւ�
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�FOBJ���W�ݒ�
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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
 * @brief		�R�}���h�FOBJ��BG�̃v���C�I���e�B�ݒ�
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�}���h�F�a�f�X�N���[���A�j��
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
 *
 * @retval	"COMM_RET_FALSE = �R�}���h�I��"
 *
 * @li	comm[0] = COMM_BG_FRM_ANIME
 * @li	comm[1] = �E�F�C�g
 */
//--------------------------------------------------------------------------------------------
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
 * @brief		�R�}���h�F�f�o�b�O�v�����g
 *
 * @param		wk		���[�N
 * @param		comm	�R�}���h�f�[�^
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

