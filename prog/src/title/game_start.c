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


//--------------------------------------------------------------
/**
 * @brief   �u�ŏ�����n�߂�v��I��
 */
//--------------------------------------------------------------
void GameStart_Beginning(void)
{
	GAME_INIT_WORK * init_param = DEBUG_GetGameInitWork(GAMEINIT_MODE_DEBUG, 0);
	GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);
}

//--------------------------------------------------------------
/**
 * @brief   �u��������n�߂�v��I��
 */
//--------------------------------------------------------------
void GameStart_Continue(void)
{
	GAME_INIT_WORK * init_param = DEBUG_GetGameInitWork(GAMEINIT_MODE_DEBUG, 0);
	GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);
}

//--------------------------------------------------------------
/**
 * @brief   �u�f�o�b�O�J�n�v��I��
 */
//--------------------------------------------------------------
void GameStart_Debug(void)
{
#ifdef PM_DEBUG
	GAME_INIT_WORK * init_param = DEBUG_GetGameInitWork(GAMEINIT_MODE_DEBUG, 0);
	GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);
#endif
}

