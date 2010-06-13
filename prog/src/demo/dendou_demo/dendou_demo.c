//============================================================================================
/**
 * @file		dendou_demo.c
 * @brief		�a������f�����
 * @author	Hiroyuki Nakamura
 * @date		10.04.06
 *
 *	���W���[�����FDENDOUDEMO
 */
//============================================================================================
#include <gflib.h>

#include "ddemo_main.h"
#include "ddemo_seq.h"



//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT DendouDemoProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT DendouDemoProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT DendouDemoProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// PROC�f�[�^
const GFL_PROC_DATA DENDOUDEMO_ProcData = {
	DendouDemoProc_Init,
	DendouDemoProc_Main,
	DendouDemoProc_End
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
static GFL_PROC_RESULT DendouDemoProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	DDEMOMAIN_WORK * wk;

//	OS_Printf( "�����������@�a������f�������J�n�@����������\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_DENDOU_DEMO, 0x60000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(DDEMOMAIN_WORK), HEAPID_DENDOU_DEMO );
	GFL_STD_MemClear( wk, sizeof(DDEMOMAIN_WORK) );

	wk->dat = pwk;

#ifdef	PM_DEBUG
	if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ){
		wk->debugMode = TRUE;
		wk->debugNext = 1;
	}
#endif	// PM_DEBUG

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
static GFL_PROC_RESULT DendouDemoProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	if( DDEMOSEQ_MainSeq( mywk ) == FALSE ){
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
static GFL_PROC_RESULT DendouDemoProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_DENDOU_DEMO );

//	OS_Printf( "�����������@�a������f�������I���@����������\n" );

	return GFL_PROC_RES_FINISH;
}
