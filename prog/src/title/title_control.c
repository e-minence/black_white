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
static COMMANDDEMO_DATA	cdemo_data;		// �f�������f�[�^


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
	case 0:		// �Ж��\��
#ifdef DEBUG_ONLY_FOR_hudson
    // HUDSON�Ŏ��s�����ꍇ�͒��Ńt�B�[���h��
    CorpRet = CORPORATE_RET_DEBUG;
    *seq = 4;
#else
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &CorpProcData, &CorpRet );
		*seq = 1;
#endif
		break;

	case 1:		// �Ж��f��
		if( CorpRet == CORPORATE_RET_NORMAL ){
			cdemo_data.mode = COMMANDDEMO_MODE_GF_LOGO;
			cdemo_data.skip = COMMANDDEMO_SKIP_DEBUG;
			cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
			GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
			*seq = 2;
		}else{
			*seq = 4;		// �^�C�g����
		}
		break;

	case 2:
		if( cdemo_data.ret != COMMANDDEMO_RET_SKIP_DEBUG ){
			cdemo_data.mode = COMMANDDEMO_MODE_OP_MOVIE1;
			cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
			GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
			*seq = 3;
		}else{
			*seq = 4;		// �^�C�g����
		}
		break;

	case 3:
		if( cdemo_data.ret != COMMANDDEMO_RET_SKIP_DEBUG ){
			cdemo_data.mode = COMMANDDEMO_MODE_OP_MOVIE2;
			cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
			GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
		}
		*seq = 4;		// �^�C�g����
		break;

	case 4:		// ���C���^�C�g��
		if( cdemo_data.ret == COMMANDDEMO_RET_SKIP_DEBUG ){
			CorpRet = CORPORATE_RET_DEBUG;
		}
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &TitleProcData, &CorpRet );
		*seq = 0;
		break;
	}

/*
	switch( *seq  ){
	case 0:		// �Ж��\��
		cdemo_data.mode = COMMANDDEMO_MODE_OP_MOVIE1;
		cdemo_data.skip = COMMANDDEMO_SKIP_DEBUG;
		cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &CorpProcData, &CorpRet );
		*seq = 1;
		break;

	case 1:		// �f��
		if( CorpRet == CORPORATE_RET_NORMAL ){
			GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
		}
		*seq = 2;
		break;

	case 2:		// ���C���^�C�g��
		if( cdemo_data.ret == COMMANDDEMO_RET_SKIP_DEBUG ){
			CorpRet = CORPORATE_RET_DEBUG;
		}
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &TitleProcData, &CorpRet );
		*seq = 0;
		break;
	}
*/
#else
	switch( *seq  ){
	case 0:		// �Ж��\��
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &CorpProcData, NULL );
		*seq = 1;
		break;

	case 1:		// �Ж��f��
		cdemo_data.mode = COMMANDDEMO_MODE_GF_LOGO;
		cdemo_data.skip = COMMANDDEMO_SKIP_ON;
		cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
		*seq = 2;
		break;

	case 2:
		cdemo_data.mode = COMMANDDEMO_MODE_OP_MOVIE1;
		cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
		*seq = 3;
		break;

	case 3:
		cdemo_data.mode = COMMANDDEMO_MODE_OP_MOVIE2;
		cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
		*seq = 4;		// �^�C�g����
		break;

	case 4:		// ���C���^�C�g��
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &TitleProcData, NULL );
		*seq = 0;
		break;
	}
/*
	switch( *seq  ){
	case 0:		// �Ж��\��
		cdemo_data.mode = COMMANDDEMO_MODE_OP_MOVIE1;
		cdemo_data.skip = COMMANDDEMO_SKIP_ON;
		cdemo_data.ret  = COMMANDDEMO_RET_NORMAL;
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &CorpProcData, NULL );
		*seq = 1;
		break;

	case 1:		// �f��
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &cdemo_data );
		*seq = 2;
		break;

	case 2:		// ���C���^�C�g��
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(title), &TitleProcData, NULL );
		*seq = 0;
		break;
	}
*/
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
