//============================================================================================
/**
 * @file	field_proc.c
 * @brief	�t�B�[���h�v���Z�X (watanabe080926
 * @author	tamada
 * @date	2007.02.01
 */
//============================================================================================
#include <gflib.h>
#include "system\main.h"

#include "field_main.h"
#include "gamesystem/gamesystem.h"

//============================================================================================
//
//
//		�v���Z�X�̒�`
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̏�����
 *
 * �����Ńq�[�v�̐�����e�평�����������s���B
 * �����i�K�ł�mywk��NULL�����AGFL_PROC_AllocWork���g�p�����
 * �ȍ~�͊m�ۂ������[�N�̃A�h���X�ƂȂ�B
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugFieldProcInit
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAMESYS_WORK * gsys = pwk;
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WATANABE_DEBUG, 0x200000 );
	FieldBoot(gsys, HEAPID_WATANABE_DEBUG );

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̃��C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugFieldProcMain
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	if( FieldMain() == TRUE ){
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̏I������
 *
 * �P�ɏI�������ꍇ�A�e�v���Z�X�ɏ������Ԃ�B
 * GFL_PROC_SysSetNextProc���Ă�ł����ƁA�I���セ�̃v���Z�X��
 * �������J�ڂ���B
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugFieldProcEnd
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	FieldEnd();
	GFL_HEAP_DeleteHeap( HEAPID_WATANABE_DEBUG );

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_PROC_DATA DebugFieldProcData = {
	DebugFieldProcInit,
	DebugFieldProcMain,
	DebugFieldProcEnd,
};




