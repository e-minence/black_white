//======================================================================
/**
 * @file	mus_comm_func.h
 * @brief	�~���[�W�J���ʐM�@�\
 * @author	ariizumi
 * @data	09/05/29
 *
 * ���W���[�����FMUS_COMM_FUNC
 */
//======================================================================
#pragma once

#include "musical/musical_define.h"
#include "musical/musical_local.h"
#include "gamesystem/game_comm.h"
#include "savedata/save_control.h"
#include "savedata/mystatus.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//�^�C�~���O�R�}���h
#define MUS_COMM_TIMMING_DRESSUP_WAIT (10)
#define MUS_COMM_TIMMING_START_ACTING (20)
//�f�o�b�O
#define MUS_COMM_TIMMING_START_SCRIPT (200)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
//�ʐM���[�h
typedef enum
{
  MCM_NONE,
  MCM_PARENT,
  MCM_CHILD,
  MUS_COMM_MODE_MAX
}MUS_COMM_MODE;


//�Q�[�����
typedef enum
{
  MCGS_NONE = 0,
//  MCGS_LOBBY,
  MCGS_DRESSUP = 1,
  MCGS_WAIT_DRESSUP = 2,
  MCGS_ACTING = 3,
  
}MUS_COMM_GAME_STATE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct
{
  STRCODE name[BUFLEN_PERSON_NAME+EOM_SIZE];
}MUS_COMM_BEACON;

typedef struct _MUS_COMM_WORK MUS_COMM_WORK;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

MUS_COMM_WORK* MUS_COMM_CreateWork( HEAPID heapId , GAME_COMM_SYS_PTR gameComm , SAVE_CONTROL_WORK *saveCtrl , MUSICAL_DISTRIBUTE_DATA *distData );
void MUS_COMM_DeleteWork( MUS_COMM_WORK* work );

void MUS_COMM_InitComm( MUS_COMM_WORK* work );
void MUS_COMM_ExitComm( MUS_COMM_WORK* work );
void MUS_COMM_UpdateComm( MUS_COMM_WORK* work );
const BOOL MUS_COMM_IsInitComm( MUS_COMM_WORK* work );

void MUS_COMM_StartParent( MUS_COMM_WORK* work );
void MUS_COMM_StartChild( MUS_COMM_WORK* work );
const BOOL MUS_COMM_StartGame( MUS_COMM_WORK* work );
const BOOL MUS_COMM_IsStartGame( MUS_COMM_WORK* work );

//�Q�[����Ԃ̐ݒ�
const BOOL MUS_COMM_SetCommGameState( MUS_COMM_WORK *work , MUS_COMM_GAME_STATE state );

const BOOL MUS_COMM_Send_MusPokeData( MUS_COMM_WORK *work , MUSICAL_POKE_PARAM *musPoke );
const BOOL MUS_COMM_Send_AllMusPokeData( MUS_COMM_WORK *work  );
const BOOL MUS_COMM_CheckAllPostPokeData( MUS_COMM_WORK *work );

void MUS_COMM_Start_SendStrmData( MUS_COMM_WORK *work );
const BOOL MUS_COMM_Send_StrmData( MUS_COMM_WORK *work , const u8 idx );

//�~���[�W�J���̖���IDX�Ńf�[�^���擾
MUSICAL_POKE_PARAM* MUS_COMM_GetMusPokeParam( MUS_COMM_WORK *work , const u8 musicalIdx );

void MUS_COMM_SendTimingCommand( MUS_COMM_WORK *work , const u8 no );
const BOOL MUS_COMM_CheckTimingCommand( MUS_COMM_WORK *work , const u8 no );

MYSTATUS *MUS_COMM_GetPlayerMyStatus( MUS_COMM_WORK* work , u8 idx );
const BOOL MUS_COMM_IsRefreshUserData( MUS_COMM_WORK *work );
void MUS_COMM_ResetRefreshUserData( MUS_COMM_WORK *work );

const BOOL MUS_COMM_Send_MusicalIndex( MUS_COMM_WORK* work );
const BOOL MUS_COMM_Send_UseButtonFlg( MUS_COMM_WORK* work , u8 pos );

u8 MUS_COMM_GetSelfMusicalIndex( MUS_COMM_WORK* work );
GAME_COMM_SYS_PTR MUS_COMM_GetGameComm( MUS_COMM_WORK* work );
MUS_COMM_MODE MUS_COMM_GetMode( MUS_COMM_WORK* work );
u8 MUS_COMM_GetUseButtonPos( MUS_COMM_WORK* work , const u8 musIdx );
void MUS_COMM_ResetUseButtonPos( MUS_COMM_WORK* work , const u8 musIdx );
u8 MUS_COMM_GetUseButtonAttention( MUS_COMM_WORK* work );
void MUS_COMM_ResetUseButtonAttention( MUS_COMM_WORK* work );
