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
#include "gamesystem/game_comm.h"

typedef struct _MUS_COMM_WORK MUS_COMM_WORK;

//���[�N�̍쐬�E�폜
//�����Ń��[�N�͂��炦�Ȃ��̂ŁA�������m�F�̌�A���炤���ƁI
extern void MUS_COMM_InitField( HEAPID heapId , GAMEDATA *gameData , GAME_COMM_SYS_PTR gameComm , const BOOL isIrc );
extern void MUS_COMM_ExitField( MUS_COMM_WORK *work );

extern void* MUS_COMM_InitGameComm(int *seq, void *pwk);
extern BOOL MUS_COMM_ExitGameComm(int *seq, void *pwk, void *pWork);
extern BOOL MUS_COMM_ExitWaitGameComm(int *seq, void *pwk, void *pWork);

extern void MUS_COMM_UpdateGameComm(int *seq, void *pwk, void *pWork);

extern void MUS_COMM_InitAfterWirelessConnect( MUS_COMM_WORK* work );
extern void MUS_COMM_InitAfterIrcConnect( MUS_COMM_WORK* work );
extern const BOOL MUS_COMM_IsPostProgramSize( const MUS_COMM_WORK *work );
extern const BOOL MUS_COMM_IsPostProgramData( const MUS_COMM_WORK *work );
extern const BOOL MUS_COMM_IsPostMessageSize( const MUS_COMM_WORK *work );
extern const BOOL MUS_COMM_IsPostMessageData( const MUS_COMM_WORK *work );
extern const BOOL MUS_COMM_IsPostScriptSize( const MUS_COMM_WORK *work );
extern const BOOL MUS_COMM_IsPostScriptData( const MUS_COMM_WORK *work );