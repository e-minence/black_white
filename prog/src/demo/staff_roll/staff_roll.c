//============================================================================================
/**
 * @file		staff_roll.c
 * @brief		�G���f�B���O�E�X�^�b�t���[�����
 * @author	Hiroyuki Nakamura
 * @date		10.04.19
 *
 *	���W���[�����FSTAFFROLL
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"

#include "staff_roll_main.h"



//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT StaffRollProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT StaffRollProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT StaffRollProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// PROC�f�[�^
const GFL_PROC_DATA STAFFROLL_ProcData = {
	StaffRollProc_Init,
	StaffRollProc_Main,
	StaffRollProc_End
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
static GFL_PROC_RESULT StaffRollProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	SRMAIN_WORK * wk;

//	OS_Printf( "�����������@�X�^�b�t���[�������J�n�@����������\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_STAFF_ROLL, 0x120000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(SRMAIN_WORK), HEAPID_STAFF_ROLL );
	GFL_STD_MemClear( wk, sizeof(SRMAIN_WORK) );

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
static GFL_PROC_RESULT StaffRollProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	if( SRMAIN_Main( mywk ) == FALSE ){
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
static GFL_PROC_RESULT StaffRollProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_STAFF_ROLL );

//	OS_Printf( "�����������@�X�^�b�t���[�������I���@����������\n" );

	return GFL_PROC_RES_FINISH;
}
