//==============================================================================
/**
 * @file	game_start.c
 * @brief	�u�������傩��vor�u�Â�����v��I����̏���
 * @author	matsuda
 * @date	2009.01.07(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include "system\main.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "title/game_start.h"
#include "savedata/situation.h"
#include "app/name_input.h"


//==============================================================================
//	
//==============================================================================
FS_EXTERN_OVERLAY(title);

//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT GameStart_FirstProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_FirstProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT GameStart_FirstProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_ContinueProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_ContinueProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_ContinueProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


//==============================================================================
//	�f�[�^
//==============================================================================
///�ŏ�����n�߂�
static const GFL_PROC_DATA GameStart_FirstProcData = {
	GameStart_FirstProcInit,
	GameStart_FirstProcMain,
	GameStart_FirstProcEnd,
};

///��������n�߂�
static const GFL_PROC_DATA GameStart_ContinueProcData = {
	GameStart_ContinueProcInit,
	GameStart_ContinueProcMain,
	GameStart_ContinueProcEnd,
};

///�f�o�b�O�J�n
static const GFL_PROC_DATA GameStart_DebugProcData = {
	GameStart_DebugProcInit,
	GameStart_DebugProcMain,
	GameStart_DebugProcEnd,
};


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �u�ŏ�����n�߂�v��I��
 */
//--------------------------------------------------------------
void GameStart_Beginning(void)
{
	GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_FirstProcData, NULL);
}

//--------------------------------------------------------------
/**
 * @brief   �u��������n�߂�v��I��
 */
//--------------------------------------------------------------
void GameStart_Continue(void)
{
	GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_ContinueProcData, NULL);
}

//--------------------------------------------------------------
/**
 * @brief   �u�f�o�b�O�J�n�v��I��
 */
//--------------------------------------------------------------
void GameStart_Debug(void)
{
#ifdef PM_DEBUG
	GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_DebugProcData, NULL);
#endif
}



//==============================================================================
//
//	�ŏ�����n�߂�
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GameStart_FirstProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	SaveControl_ClearData(SaveControl_GetPointer());	//�Z�[�u�f�[�^�N���A
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_FirstProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	switch(*seq){
	case 0:
		GFL_PROC_SysCallProc(NO_OVERLAY_ID, &NameInputProcData,(void*)NAMEIN_MYNAME);
		(*seq)++;
		break;
	case 1:
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_FirstProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAME_INIT_WORK * init_param;
	VecFx32 pos = {0,0,0};
	
	init_param = DEBUG_GetGameInitWork(GAMEINIT_MODE_FIRST, 0, &pos, 0);
	GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);

	return GFL_PROC_RES_FINISH;
}



//==============================================================================
//
//	��������n�߂�
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GameStart_ContinueProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_ContinueProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_ContinueProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAME_INIT_WORK * init_param;
	PLAYERWORK_SAVE plsv;
	
	SaveControl_Load(SaveControl_GetPointer());
	SaveData_SituationLoad_PlayerWorkSave(SaveControl_GetPointer(), &plsv);

	init_param = DEBUG_GetGameInitWork(
		GAMEINIT_MODE_CONTINUE, plsv.zoneID, &plsv.position, plsv.direction);
	
	GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);

	return GFL_PROC_RES_FINISH;
}



//==============================================================================
//
//	�f�o�b�O�X�^�[�g
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
#ifdef PM_DEBUG
	GAME_INIT_WORK * init_param;
	VecFx32 pos = {0,0,0};

	SaveControl_ClearData(SaveControl_GetPointer());
	init_param = DEBUG_GetGameInitWork(GAMEINIT_MODE_DEBUG, 0, &pos, 0);
	GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);
#endif

	return GFL_PROC_RES_FINISH;
}
