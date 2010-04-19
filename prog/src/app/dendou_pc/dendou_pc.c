//============================================================================================
/**
 * @file		dendou_pc.c
 * @brief		�a������m�F���
 * @author	Hiroyuki Nakamura
 * @date		10.03.29
 *
 *	���W���[�����FDENDOUPC
 */
//============================================================================================
#include <gflib.h>

#include "dpc_main.h"
#include "dpc_seq.h"



//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT DendouPcProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT DendouPcProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT DendouPcProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// PROC�f�[�^
const GFL_PROC_DATA DENDOUPC_ProcData = {
	DendouPcProc_Init,
	DendouPcProc_Main,
	DendouPcProc_End
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
static GFL_PROC_RESULT DendouPcProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	DPCMAIN_WORK * wk;

//	OS_Printf( "�����������@�a������o�b�����J�n�@����������\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_DENDOU_PC, 0x60000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(DPCMAIN_WORK), HEAPID_DENDOU_PC );
	GFL_STD_MemClear( wk, sizeof(DPCMAIN_WORK) );

	wk->dat = pwk;

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
static GFL_PROC_RESULT DendouPcProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	if( DPCSEQ_MainSeq( mywk ) == FALSE ){
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
static GFL_PROC_RESULT DendouPcProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_DENDOU_PC );

//	OS_Printf( "�����������@�a������o�b�����I���@����������\n" );

	return GFL_PROC_RES_FINISH;
}
