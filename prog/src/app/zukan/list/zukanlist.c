//============================================================================================
/**
 * @file		zukanlist.c
 * @brief		�}�Ӄ��X�g���
 * @author	Hiroyuki Nakamura
 * @date		09.12.15
 *
 *	���W���[�����FZUKANLIST
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"

#include "zknlist_main.h"
#include "zknlist_seq.h"


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT ZukanListProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT ZukanListProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT ZukanListProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// PROC�f�[�^
const GFL_PROC_DATA ZUKANLIST_ProcData = {
	ZukanListProc_Init,
	ZukanListProc_Main,
	ZukanListProc_End
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
static GFL_PROC_RESULT ZukanListProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	ZKNLISTMAIN_WORK * wk;

	OS_Printf( "�����������@�}�Ӄ��X�g�����J�n�@����������\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ZUKAN_LIST, 0x60000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(ZKNLISTMAIN_WORK), HEAPID_ZUKAN_LIST );
	GFL_STD_MemClear( wk, sizeof(ZKNLISTMAIN_WORK) );

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
static GFL_PROC_RESULT ZukanListProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	if( ZKNLISTSEQ_MainSeq( mywk ) == FALSE ){
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
static GFL_PROC_RESULT ZukanListProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_ZUKAN_LIST );

	OS_Printf( "�����������@�}�Ӄ��X�g�����I���@����������\n" );

	return GFL_PROC_RES_FINISH;
}
