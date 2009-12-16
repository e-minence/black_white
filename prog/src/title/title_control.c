//==============================================================================
/**
 * @file	title_control.c
 * @brief	�^�C�g����PROC����
 * @author	matsuda
 * @date	2008.12.05(��)
 *
 * �t�@�C�����Ƌ@�\����v���Ă��Ȃ��悤�ȋC������
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include "system\main.h"
#include "title/title.h"
#include "demo/command_demo.h"
#include "corporate.h"


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT TitleControlProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT TitleControlProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT TitleControlProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

//==============================================================================
//	�f�[�^
//==============================================================================

const GFL_PROC_DATA TitleControlProcData = {
	TitleControlProcInit,
	TitleControlProcMain,
	TitleControlProcEnd,
};

#ifdef PM_DEBUG
static u32 CorpRet;										// �Ж��\���������[�N
#endif
static COMMANDDEMO_DATA	cdemo_data;		// �f���������[�N


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   proc		
 * @param   seq		
 * @param   pwk		
 * @param   mywk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT TitleControlProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT TitleControlProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
#ifdef PM_DEBUG
	switch( *seq  ){
	case 0:
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &CorpProcData, &CorpRet );
		*seq = 1;
		break;

	case 1:
		if( CorpRet == CORPORATE_RET_NORMAL ){
			GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
		}
		*seq = 2;
		break;

	case 2:
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &TitleProcData, &CorpRet );
		return GFL_PROC_RES_FINISH;
	}
#else
	switch( *seq  ){
	case 0:
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &CorpProcData, NULL );
		*seq = 1;
		break;

	case 1:
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
		*seq = 2;
		break;

	case 2:
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &TitleProcData, NULL );
		return GFL_PROC_RES_FINISH;
	}
#endif

	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT TitleControlProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	return GFL_PROC_RES_FINISH;
}
