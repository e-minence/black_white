//============================================================================================
/**
 * @file	fieldmap_proc.c
 * @brief	�t�B�[���h�v���Z�X 
 * @author	tamada
 * @date	2008.11.18
 */
//============================================================================================
#include <gflib.h>
#include "system/main.h"				//HEAPID�Q�Ƃ̂���

#include "gamesystem/gamesystem.h"
#include "field/fieldmap.h"

//============================================================================================
//
//
//		�v���Z�X�̒�`
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	FIELD_MAIN_WORK * fieldWork;
}FPROC_WORK;
//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̏�����
 *
 * �����Ńq�[�v�̐�����e�평�����������s���B
 * �����i�K�ł�mywk��NULL�����AGFL_PROC_AllocWork���g�p�����
 * �ȍ~�͊m�ۂ������[�N�̃A�h���X�ƂȂ�B
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT FieldMapProcInit
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	FIELD_MAIN_WORK * fieldWork;
	FPROC_WORK * fpwk;
	GAMESYS_WORK * gsys = pwk;
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_FIELDMAP, 0x200000 );
	fpwk = GFL_PROC_AllocWork(proc, sizeof(FPROC_WORK), HEAPID_FIELDMAP);
	fpwk->fieldWork = FIELDMAP_Create(gsys, HEAPID_FIELDMAP );

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̃��C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT FieldMapProcMain
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAMESYS_WORK * gsys = pwk;
	FPROC_WORK * fpwk = mywk;
	if( FIELDMAP_Main(gsys, fpwk->fieldWork) == TRUE ){
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
static GFL_PROC_RESULT FieldMapProcEnd
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	FPROC_WORK * fpwk = mywk;
	FIELDMAP_Delete(fpwk->fieldWork);
	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap( HEAPID_FIELDMAP );

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_PROC_DATA FieldMapProcData = {
	FieldMapProcInit,
	FieldMapProcMain,
	FieldMapProcEnd,
};




