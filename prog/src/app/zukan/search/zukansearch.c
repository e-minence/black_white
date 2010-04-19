//============================================================================================
/**
 * @file		zukansearch.c
 * @brief		�}�ӌ������
 * @author	Hiroyuki Nakamura
 * @date		10.02.09
 *
 *	���W���[�����FZUKANSEARCH
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"

#include "zknsearch_main.h"
#include "zknsearch_seq.h"


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT ZukanSearchProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT ZukanSearchProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT ZukanSearchProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// PROC�f�[�^
const GFL_PROC_DATA ZUKANSEARCH_ProcData = {
	ZukanSearchProc_Init,
	ZukanSearchProc_Main,
	ZukanSearchProc_End
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
static GFL_PROC_RESULT ZukanSearchProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	ZKNSEARCHMAIN_WORK * wk;

//	OS_Printf( "�����������@�}�ӌ��������J�n�@����������\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ZUKAN_SEARCH, 0x60000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(ZKNSEARCHMAIN_WORK), HEAPID_ZUKAN_SEARCH );
	GFL_STD_MemClear( wk, sizeof(ZKNSEARCHMAIN_WORK) );

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
static GFL_PROC_RESULT ZukanSearchProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	if( ZKNSEARCHSEQ_MainSeq( mywk ) == FALSE ){
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
static GFL_PROC_RESULT ZukanSearchProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_ZUKAN_SEARCH );

//	OS_Printf( "�����������@�}�ӌ��������I���@����������\n" );

	return GFL_PROC_RES_FINISH;
}
