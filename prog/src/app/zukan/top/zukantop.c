//============================================================================================
/**
 * @file		zukantop.c
 * @brief		�}�Ӄg�b�v���
 * @author	Hiroyuki Nakamura
 * @date		10.02.20
 *
 *	���W���[�����FZUKANTOP
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"

#include "zkntop_main.h"


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT ZukanTopProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT ZukanTopProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT ZukanTopProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// PROC�f�[�^
const GFL_PROC_DATA ZUKANTOP_ProcData = {
	ZukanTopProc_Init,
	ZukanTopProc_Main,
	ZukanTopProc_End
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
static GFL_PROC_RESULT ZukanTopProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	ZKNTOPMAIN_WORK * wk;

//	OS_Printf( "�����������@�}�Ӄg�b�v�����J�n�@����������\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ZUKAN_TOP, 0x60000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(ZKNTOPMAIN_WORK), HEAPID_ZUKAN_TOP );
	GFL_STD_MemClear( wk, sizeof(ZKNTOPMAIN_WORK) );

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
static GFL_PROC_RESULT ZukanTopProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	if( ZKNTOPMAIN_MainSeq( mywk ) == FALSE ){
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
static GFL_PROC_RESULT ZukanTopProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_ZUKAN_TOP );

//	OS_Printf( "�����������@�}�Ӄg�b�v�����I���@����������\n" );

	return GFL_PROC_RES_FINISH;
}
