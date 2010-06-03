//============================================================================================
/**
 * @file		cdemo_seq.h
 * @brief		�R�}���h�f����� �V�[�P���X����
 * @author	Hiroyuki Nakamura
 * @date		09.05.07
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/main.h"
#include "system/wipe.h"

#include "cdemo_main.h"
#include "cdemo_seq.h"
#include "cdemo_comm.h"
#include "op_demo.naix"


//============================================================================================
//	�萔��`
//============================================================================================
#define	CDEMO_SKIP_KEY		(PAD_BUTTON_A|PAD_BUTTON_B|PAD_BUTTON_START)		// �X�L�b�v�{�^��

#define	BGFRM_ANM_FILE_OPEN_TICK			( 66667 )
#define	BGFRM_ANM_FILE_OPEN_TICK_1ST	( 56862 )		// ��̃t�@�C����ǂݍ��ނ��߂̎���

typedef struct {
	u16	arcID;					// �A�[�N�h�c
	u16	frmMax;					// �t���[����
	u32	byteSize;				// �o�C�g�T�C�Y
	u32	byteOffset;			// �]���J�n�I�t�Z�b�g
	OSTick	waitTick;		// ��̃t�@�C����ǂݍ��ނ��߂̎���
}BGFRM_ANM_DATA;
/*
42760224
 5333333
48093557
*/

//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static int MainSeq_Init( CDEMO_WORK * wk );
static int MainSeq_Release( CDEMO_WORK * wk );
static int MainSeq_Fade( CDEMO_WORK * wk );
static int MainSeq_AlphaBlend( CDEMO_WORK * wk );
static int MainSeq_BgScrnAnm( CDEMO_WORK * wk );
static int MainSeq_Main( CDEMO_WORK * wk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// ���C���V�[�P���X
static const pCommDemoFunc MainSeq[] = {
	MainSeq_Init,
	MainSeq_Release,
	MainSeq_Fade,
	MainSeq_AlphaBlend,
	MainSeq_BgScrnAnm,

	MainSeq_Main,
};

// ���[�h�ʃf�[�^
static const BGFRM_ANM_DATA ModeData[] =
{
	{	ARCID_GF_LOGO_MOVIE, 105, 256*192*2, 0, BGFRM_ANM_FILE_OPEN_TICK },					// �Q�[���t���[�N���S
	{	ARCID_CDEMO_DATA, 752, 256*170*2, 256*11*2, BGFRM_ANM_FILE_OPEN_TICK_1ST },	// �I�[�v�j���O���[�r�[�P
//	{	ARCID_CDEMO_DATA, 752, 256*170*2, 256*11*2, BGFRM_ANM_FILE_OPEN_TICK },	// �I�[�v�j���O���[�r�[�P
	{	ARCID_OP_DEMO2_MOVIE, 174, 256*192*2, 0, BGFRM_ANM_FILE_OPEN_TICK },				// �I�[�v�j���O���[�r�[�Q
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		���C������
 *
 * @param		wk		�f�����[�N
 *
 * @retval	"TRUE = ������"
 * @retval	"FALSE = ����ȊO"
 */
//-------------------------------------------------------------------------------------------
BOOL CDEMOSEQ_Main( CDEMO_WORK * wk, int * seq )
{
	if( wk->dat->skip != COMMANDDEMO_SKIP_OFF ){
		if( GFL_UI_KEY_GetTrg() & CDEMO_SKIP_KEY ){
			wk->main_seq = CDEMOSEQ_MAIN_RELEASE;
			wk->dat->ret = COMMANDDEMO_RET_SKIP;
		}
	}
#ifdef PM_DEBUG
	if( wk->dat->skip == COMMANDDEMO_SKIP_DEBUG ){
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
			wk->main_seq = CDEMOSEQ_MAIN_RELEASE;
			wk->dat->ret = COMMANDDEMO_RET_SKIP_DEBUG;
		}
	}
#endif

	wk->main_seq = MainSeq[wk->main_seq]( wk );

	if( wk->main_seq == CDEMOSEQ_MAIN_END ){
		return FALSE;
	}

	return TRUE;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F������
 *
 * @param		wk		�f�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Init( CDEMO_WORK * wk )
{
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �T�u��ʂ����C����
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );

	CDEMOMAIN_VramBankSet();
	CDEMOMAIN_BgInit( wk );

	wk->gra_ah = GFL_ARC_OpenDataHandle( ModeData[wk->dat->mode].arcID, HEAPID_COMMAND_DEMO ); 

	// �f�[�^���[�h
	CDEMOMAIN_CommDataLoad( wk );

//	WIPE_Reset( WIPE_DISP_SUB );

	CDEMOMAIN_InitVBlank( wk );

//	return CDEMOSEQ_MAIN_MAIN;
	CDEMOMAIN_FadeInSet( wk, 1, 1, CDEMOSEQ_MAIN_MAIN );

	wk->init_flg = TRUE;

	return CDEMOSEQ_MAIN_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���
 *
 * @param	wk		�f�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Release( CDEMO_WORK * wk )
{
	if( WIPE_SYS_EndCheck() == FALSE ){
		return CDEMOSEQ_MAIN_RELEASE;
	}

	// ���������Ă�����
	if( wk->init_flg == TRUE ){
		CDEMOMAIN_ExitVBlank( wk );
		GFL_ARC_CloseDataHandle( wk->gra_ah );	
		CDEMOMAIN_CommDataDelete( wk );
		CDEMOMAIN_BgExit();
	}

	return CDEMOSEQ_MAIN_END;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X�F�t�F�[�h
 *
 * @param	wk		�`���f�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Fade( CDEMO_WORK * wk )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		return wk->next_seq;
	}
	return CDEMOSEQ_MAIN_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F�A���t�@�u�����f�B���O
 *
 * @param		wk		�f�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_AlphaBlend( CDEMO_WORK * wk )
{
/*
	LDA_WORK * awk;
	int	ev1, ev2;

	awk = wk->work;

	if( wk->cnt == awk->alpha_mv_frm ){
		ev1 = awk->alpha_end_ev;
		ev2 = abs( 15 - ev1 );
		G2_SetBlendAlpha( awk->alpha_plane1, awk->alpha_plane2, ev1, ev2 );
		wk->cnt = 0;
		return wk->next_seq;
	}

	ev1 = ( ( abs(awk->alpha_ev1-awk->alpha_end_ev) << 8 ) / awk->alpha_mv_frm * wk->cnt ) >> 8;
	if( awk->alpha_ev1 > awk->alpha_end_ev ){
		ev1 = awk->alpha_ev1 - ev1;
	}else{
		ev1 = awk->alpha_ev1 + ev1;
	}
	ev2 = abs( 15 - ev1 );

	G2_SetBlendAlpha( awk->alpha_plane1, awk->alpha_plane2, ev1, ev2 );

	wk->cnt++;
*/

	return CDEMOSEQ_MAIN_ALPHA_BLEND;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�F���C��
 *
 * @param		wk		�f�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Main( CDEMO_WORK * wk )
{
	while(1){
		int	ret;
		
		if( wk->commPos[0] == CDEMOCOMM_END ){
			wk->dat->ret = COMMANDDEMO_RET_NORMAL;
			return CDEMOSEQ_MAIN_RELEASE;
		}

		ret = CDEMOCOMM_Main( wk );
		
		if( ret != CDEMOCOMM_RET_FALSE ){
			break;
		}
	}

	return wk->main_seq;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���V�[�P���X�FBG�X�N���[���A�j��
 *
 * @param		wk		�f�����[�N
 *
 * @return	���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_BgScrnAnm( CDEMO_WORK * wk )
{
	const BGFRM_ANM_DATA * dat = &ModeData[wk->dat->mode];

/*
	if( wk->bfTick == 0 ){
		wk->bfTick = OS_GetTick();
	}else{
		wk->afTick = OS_GetTick();
		wk->stTick = wk->stTick + OS_TicksToMicroSeconds( wk->afTick - wk->bfTick );
		wk->bfTick = wk->afTick;
		if( wk->stTick < BGFRM_ANM_FILE_OPEN_TICK ){
			return CDEMOSEQ_MAIN_BG_SCRN_ANM;
		}else{
			wk->stTick -= BGFRM_ANM_FILE_OPEN_TICK;
		}
	}
*/
	if( wk->bfTick == 0 ){
		wk->bfTick = OS_GetTick();
	}else{
		wk->afTick = OS_GetTick();
		wk->stTick = wk->stTick + OS_TicksToMicroSeconds( wk->afTick - wk->bfTick );
		wk->bfTick = wk->afTick;
		if( wk->stTick < dat->waitTick ){
			return CDEMOSEQ_MAIN_BG_SCRN_ANM;
		}else{
			wk->stTick -= dat->waitTick;
		}
	}

	switch( wk->bgsa_seq ){
	case 0:
		{
			void * buf;
//			buf = GFL_ARCHDL_UTIL_Load( wk->gra_ah, NARC_op_demo_op001_LZ_bin, TRUE, HEAPID_COMMAND_DEMO_L );
			buf = GFL_ARCHDL_UTIL_Load( wk->gra_ah, 0, TRUE, HEAPID_COMMAND_DEMO_L );
			DC_FlushRange( buf, dat->byteSize );
	    GX_LoadBG2Bmp( buf, dat->byteOffset, dat->byteSize );
			GFL_HEAP_FreeMemory( buf );
			wk->bgsa_num += 1;
//			buf = GFL_ARCHDL_UTIL_Load( wk->gra_ah, NARC_op_demo_op001_LZ_bin+wk->bgsa_num, TRUE, HEAPID_COMMAND_DEMO_L );
			buf = GFL_ARCHDL_UTIL_Load( wk->gra_ah, wk->bgsa_num, TRUE, HEAPID_COMMAND_DEMO_L );
			DC_FlushRange( buf, dat->byteSize );
	    GX_LoadBG3Bmp( buf, dat->byteOffset, dat->byteSize );
			GFL_HEAP_FreeMemory( buf );
			wk->bgsa_num += 1;
		}
		GFL_BG_SetPriority( GFL_BG_FRAME2_M, 0 );
		GFL_BG_SetPriority( GFL_BG_FRAME3_M, 1 );
		wk->bgsa_load++;
		wk->bgsa_seq = 1;

	case 1:
		if( wk->bgsa_cnt >= wk->bgsa_wait ){
			if( wk->bgsa_num >= dat->frmMax ){
				return CDEMOSEQ_MAIN_MAIN;
			}
			wk->bgsa_cnt = 0;
			wk->bgsa_seq = 2;
			break;
		}
		wk->bgsa_cnt++;
		break;

	case 2:
		GFL_BG_SetPriority( GFL_BG_FRAME2_M+(wk->bgsa_load&1), 0 );
		GFL_BG_SetPriority( GFL_BG_FRAME2_M+((wk->bgsa_load+1)&1), 1 );
		if( wk->bgsa_num < dat->frmMax ){
			void * buf;
//			buf = GFL_ARCHDL_UTIL_Load( wk->gra_ah, NARC_op_demo_op001_LZ_bin+wk->bgsa_num, TRUE, HEAPID_COMMAND_DEMO_L );
			buf = GFL_ARCHDL_UTIL_Load( wk->gra_ah, wk->bgsa_num, TRUE, HEAPID_COMMAND_DEMO_L );
			DC_FlushRange( buf, dat->byteSize );
			if( ((wk->bgsa_load+1)&1) == 0 ){
		    GX_LoadBG2Bmp( buf, dat->byteOffset, dat->byteSize );
			}else{
		    GX_LoadBG3Bmp( buf, dat->byteOffset, dat->byteSize );
			}
			GFL_HEAP_FreeMemory( buf );
			wk->bgsa_num += 1;
		}
		wk->bgsa_load++;
		wk->bgsa_seq = 3;

	case 3:
		if( wk->bgsa_cnt >= wk->bgsa_wait ){
			if( wk->bgsa_num >= dat->frmMax ){
				return CDEMOSEQ_MAIN_MAIN;
			}
			wk->bgsa_cnt = 0;
			wk->bgsa_seq = 2;
			break;
		}
		wk->bgsa_cnt++;
		break;
	}

//	OS_Printf( "seq tick = %d\n", OS_TicksToMicroSeconds( OS_GetTick() ) );

	return CDEMOSEQ_MAIN_BG_SCRN_ANM;
}

