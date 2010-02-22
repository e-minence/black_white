//============================================================================================
/**
 * @file		time_icon.c
 * @brief		�Z�[�u���Ȃǂɕ\�������^�C�}�[�A�C�R������
 * @author	Hiroyuki Nakamura
 * @date		10.02.22
 *
 *	���W���[�����FTIMEICON
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/time_icon.h"
#include "time_icon_gra.naix"


//============================================================================================
//	�萔��`
//============================================================================================

// ���[�N
struct _TIMEICON_WORK {
	GFL_TCB * tcb;			// TCB
	GFL_BMPWIN * win;		// BMPWIN
	GFL_BMP_DATA * src;	// �A�C�R���L�����f�[�^
	HEAPID heapID;			// �q�[�v�h�c

	u8	col;						// �N���A�J���[
	u8	cnt;
	u8	wait;
	u8	anm;

	BOOL	flg;					// ���싖�t���O
};

#define	TIMEICON_SX				( 2 )								// �A�C�R���w�T�C�Y�i�L�����P�ʁj
#define	TIMEICON_SY				( 2 )								// �A�C�R���x�T�C�Y�i�L�����P�ʁj
#define	TIMEICON_DOT_SX		( TIMEICON_SX*8 )		// �A�C�R���w�T�C�Y�i�h�b�g�P�ʁj
#define	TIMEICON_DOT_SY		( TIMEICON_SY*8 )		// �A�C�R���x�T�C�Y�i�h�b�g�P�ʁj

#define	TIMEICON_ANM_MAX	( 8 )															// �A�C�R���A�j����


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void MainTask(  GFL_TCB * tcb, void * work );


//--------------------------------------------------------------------------------------------
/**
 * �^�C�}�[�A�C�R�������ǉ�
 *
 * @param		tcbsys				GFL_TCBSYS
 * @param		msg_win				���b�Z�[�W��\�����Ă���E�B���h�E
 * @param		clear_color		�h��Ԃ��J���[
 * @param		wait					�A�j���E�F�C�g
 * @param		heapID				�q�[�v�h�c
 *
 * @return	���[�N
 */
//--------------------------------------------------------------------------------------------
TIMEICON_WORK * TIMEICON_Create(
									GFL_TCBSYS * tcbsys, GFL_BMPWIN * msg_win, u8 clear_color, u8 wait, HEAPID heapID )
{
	TIMEICON_WORK * wk = GFL_HEAP_AllocMemory( heapID, sizeof(TIMEICON_WORK) );

	wk->heapID = heapID;
	wk->col    = clear_color;
	wk->cnt    = 0;
	wk->wait   = wait;
	wk->anm    = 0;

	wk->src = GFL_BMP_LoadCharacter(
							ARCID_TIMEICON_GRA, NARC_time_icon_gra_time_icon_lz_NCGR, TRUE, wk->heapID );
	wk->win = GFL_BMPWIN_Create(
							GFL_BMPWIN_GetFrame(msg_win),
							GFL_BMPWIN_GetPosX(msg_win)+GFL_BMPWIN_GetSizeX(msg_win)-TIMEICON_SX,
							GFL_BMPWIN_GetPosY(msg_win)+GFL_BMPWIN_GetSizeY(msg_win)-TIMEICON_SY,
							TIMEICON_SX, TIMEICON_SY,
							GFL_BMPWIN_GetPalette(msg_win),
							GFL_BMP_CHRAREA_GET_B );

	GFL_BMPWIN_MakeScreen( wk->win );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(wk->win) );

  wk->tcb = GFL_TCB_AddTask( tcbsys, MainTask, wk, 0 );
	wk->flg = TRUE;

	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * �^�C�}�[�A�C�R�������폜
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void TILEICON_Exit( TIMEICON_WORK * wk )
{
	wk->flg = FALSE;

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win), wk->col );
	GFL_BMPWIN_TransVramCharacter( wk->win );

	GFL_BMPWIN_Delete( wk->win );
	GFL_BMP_Delete( wk->src );
	GFL_TCB_DeleteTask( wk->tcb );
	GFL_HEAP_FreeMemory( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���^�X�N
 *
 * @param		tcb			GFL_TCB
 * @param		work		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainTask( GFL_TCB * tcb, void * work )
{
	TIMEICON_WORK * wk = work;

	if( wk->flg == FALSE ){ return; }

	if( wk->cnt == 0 ){
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win), wk->col );
		GFL_BMP_Print(
			wk->src,
			GFL_BMPWIN_GetBmp(wk->win),
			0, TIMEICON_DOT_SY * wk->anm,
			0, 0,
      TIMEICON_DOT_SX, TIMEICON_DOT_SY,
			0 );
		GFL_BMPWIN_TransVramCharacter( wk->win );
		wk->cnt = wk->wait;
		wk->anm++;
		if( wk->anm == TIMEICON_ANM_MAX ){
			wk->anm = 0;
		}
	}else{
		wk->cnt--;
	}
}
