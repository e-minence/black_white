//==============================================================================
/**
 * @file	game_start.h
 * @brief	�u�������傩��vor�u�Â�����v��I����̏���
 * @author	matsuda
 * @date	2009.01.07(��)
 */
//==============================================================================
#ifndef __GAME_START_H__
#define __GAME_START_H__
#include "playable_version.h"
//==============================================================================
//	�O���֐��錾
//==============================================================================
extern void GameStart_Beginning(void);
extern void GameStart_Continue(void);
extern void GameStart_ContinueNetOff(void);
extern void GameStart_ContinueNet(void);
extern void GameStart_Debug(void);
extern void GameStart_DebugNet(void);
extern void GameStart_DebugNetOff(void);
extern void GameStart_Debug_SelectName(void);

#ifdef  PLAYABLE_VERSION
extern void GameStart_Playable(void);
#endif


#endif	//__GAME_START_H__
