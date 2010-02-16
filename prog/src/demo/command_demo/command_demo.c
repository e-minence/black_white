//============================================================================================
/**
 * @file		command_demo.c
 * @brief		�R�}���h�f����� �Ăяo��
 * @author	Hiroyuki Nakamura
 * @date		09.01.21
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"

#include "cdemo_main.h"
#include "cdemo_seq.h"

/*
#include "common.h"

#include "ld_main.h"
#include "ld_lugia.h"
#include "ld_houou.h"
#include "ld_arceus.h"
*/


//============================================================================================
//	�萔��`
//============================================================================================


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT CommDemoProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT CommDemoProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT CommDemoProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


//============================================================================================
//	�O���[�o���ϐ�
//============================================================================================

const GFL_PROC_DATA COMMANDDEMO_ProcData = {
	CommDemoProc_Init,
	CommDemoProc_Main,
	CommDemoProc_End,
};

/*
// ���C������
static const pLegendDemoFunc MainFunc[] = {
	LDHOUOU_Main,		// �z�E�I�E
	LDLUGIA_Main,		// ���M�A
	LDARCEUS_Main,		// �A���Z�E�X
};
*/



//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F������
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT CommDemoProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
/*
	LEGEND_DEMO_WORK * wk;

	OS_Printf( "�����������@�`���f�������J�n�@����������\n" );

	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_LEGEND_DEMO, 0x80000 );

	wk = PROC_AllocWork( proc, sizeof(LEGEND_DEMO_WORK), HEAPID_LEGEND_DEMO );
	MI_CpuClear8( wk, sizeof(LEGEND_DEMO_WORK) );

	wk->dat = PROC_GetParentWork( proc );

	return PROC_RES_FINISH;
*/

/*
	BOX2_SYS_WORK * syswk;

	OS_Printf( "�����������@�{�b�N�X�����J�n�@����������\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BOX_SYS, 0x10000 );

	syswk = GFL_PROC_AllocWork( proc, sizeof(BOX2_SYS_WORK), HEAPID_BOX_SYS );
	GFL_STD_MemClear( syswk, sizeof(BOX2_SYS_WORK) );

	syswk->dat      = pwk;
	syswk->tray     = BOXDAT_GetCureentTrayNumber( syswk->dat->sv_box );
	syswk->next_seq = BOX2SEQ_MAINSEQ_START;
*/
	CDEMO_WORK * wk;

	OS_Printf( "�����������@�R�}���h�f�������J�n�@����������\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_COMMAND_DEMO, 0x80000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(CDEMO_WORK), HEAPID_COMMAND_DEMO );
	GFL_STD_MemClear( wk, sizeof(CDEMO_WORK) );

	wk->dat = pwk;
	wk->init_flg = FALSE;

	OS_InitTick();

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F���C��
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT CommDemoProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
/*
	LEGEND_DEMO_WORK * wk = PROC_GetWork( proc );

	if( MainFunc[wk->dat->mode]( wk ) == 0 ){
		return PROC_RES_FINISH;
	}

	return PROC_RES_CONTINUE;
*/

	if( CDEMOSEQ_Main( mywk, seq ) == FALSE ){
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
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT CommDemoProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
/*
	PROC_FreeWork( proc );

	sys_DeleteHeap( HEAPID_LEGEND_DEMO );

	OS_Printf( "�����������@�`���f�������I���@����������\n" );

	return PROC_RES_FINISH;
*/

	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_BOX_SYS );

	OS_Printf( "�����������@�R�}���h�f�������I���@����������\n" );

	return GFL_PROC_RES_FINISH;
}
