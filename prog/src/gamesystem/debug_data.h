//============================================================================================
/**
 * @file	debug_data.h
 * @brief	�Q�[���J�n���̃f�o�b�O�f�[�^�ǉ�����
 * @author	tamada GAME FREAK Inc.
 * @date	09.10.24
 *
 */
//============================================================================================
#pragma once
#include "playable_version.h"
#include <gflib.h>
#include "gamesystem/game_data.h"

extern void DEBUG_SetStartData( GAMEDATA * gamedata, HEAPID heapID );

#ifdef  PLAYABLE_VERSION
extern void PLAYABLE_SetStartData( GAMEDATA * gamedata, HEAPID heapID );
#endif
