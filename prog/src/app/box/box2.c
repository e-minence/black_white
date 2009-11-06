//============================================================================================
/**
 * @file		box2.c
 * @brief		�{�b�N�X���
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 *
 *	���W���[�����FBOX2
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "app/box2.h"

/*
#include "savedata/config.h"
*/

#include "box2_seq.h"
#include "box2_main.h"


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT Box2Proc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT Box2Proc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT Box2Proc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


//============================================================================================
//	�O���[�o���ϐ�
//============================================================================================
const GFL_PROC_DATA BOX2_ProcData = {
	Box2Proc_Init,
	Box2Proc_Main,
	Box2Proc_End
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F������
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 * @param		pwk		�e���[�N
 * @param		mywk	���샏�[�N
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT Box2Proc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	BOX2_SYS_WORK * syswk;

	OS_Printf( "�����������@�{�b�N�X�����J�n�@����������\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BOX_SYS, 0x10000 );

	syswk = GFL_PROC_AllocWork( proc, sizeof(BOX2_SYS_WORK), HEAPID_BOX_SYS );
	GFL_STD_MemClear( syswk, sizeof(BOX2_SYS_WORK) );

	syswk->dat      = pwk;
	syswk->tray     = BOXDAT_GetCureentTrayNumber( syswk->dat->sv_box );
	syswk->trayMax  = BOXDAT_GetTrayMax( syswk->dat->sv_box );
	syswk->next_seq = BOX2SEQ_MAINSEQ_START;

	// �g���C�̊J��
	if( syswk->trayMax != BOX_MAX_TRAY ){
		u16	max;
		u16	i;
		if( syswk->trayMax == BOX_MIN_TRAY ){
			max = BOX_MIN_TRAY;
		}else if( syswk->trayMax == BOX_MED_TRAY ){
			max = BOX_MED_TRAY;
		}
		for( i=0; i<max; i++ ){
			if( BOXDAT_GetPokeExistCount( syswk->dat->sv_box, i ) == 0 ){
				break;
			}
		}
		if( i == max ){
			syswk->trayMax = BOXDAT_AddTrayMax( syswk->dat->sv_box );
		}
	}


/*
	syswk->box    = SaveData_GetBoxData( syswk->dat->savedata );
	syswk->party  = SaveData_GetTemotiPokemon( syswk->dat->savedata );
	syswk->item   = SaveData_GetMyItem( syswk->dat->savedata );
	syswk->config = SaveData_GetConfig( syswk->dat->savedata );

	syswk->get_pos = BOX2MAIN_GETPOS_NONE;

	syswk->cur_rcv_pos = 0;
*/

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F���C��
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 * @param		pwk		�e���[�N
 * @param		mywk	���샏�[�N
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT Box2Proc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	if( BOX2SEQ_Main( mywk, seq ) == FALSE ){
		return GFL_PROC_RES_FINISH;
	}
	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F�I��
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 * @param		pwk		�e���[�N
 * @param		mywk	���샏�[�N
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT Box2Proc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_BOX_SYS );

	OS_Printf( "�����������@�{�b�N�X�����I���@����������\n" );

/*
	BOX2_SYS_WORK * syswk = GFL_PROC_GetWork( proc );

	BOXDAT_SetCureentTrayNumber( syswk->box, syswk->tray );		// �J�����g�g���C�X�V
*/

	return GFL_PROC_RES_FINISH;
}
