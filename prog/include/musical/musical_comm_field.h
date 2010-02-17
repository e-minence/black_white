//======================================================================
/**
 * @file	musical_comm_field.h
 * @brief	�~���[�W�J���p �t�B�[���h�X�N���v�g����ĂԒʐM����
 * @author	ariizumi
 * @data	10/02/16
 */
//======================================================================
#pragma once

#include "gamesystem/game_data.h"

typedef struct _MUS_COMM_WORK MUS_COMM_WORK;

//���[�N�̍쐬�E�폜
extern MUS_COMM_WORK* MUS_COMM_InitField( HEAPID heapId , GAMEDATA *gameData );
extern void MUS_COMM_ExitField( MUS_COMM_WORK *work );
