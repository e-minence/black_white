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
#include "musical/musical_comm_field.h"
#include "poke_tool/poke_tool.h"
#include "gamesystem/game_comm.h"
#include "savedata/save_control.h"
#include "savedata/mystatus.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//�A�s�[���{�[�X�̕ێ���
#define MUS_COMM_APPEALBONUS_NUM (2)
#define MUS_COMM_APPEALBONUS_INVALID (0xFF)

//�^�C�~���O�R�}���h
//musical_scr_local.h�ɂ��^�C�~���O��`����̂Œ���
#define MUS_COMM_SYNC_START_SEND_PROGRAM (64)
#define MUS_COMM_SYNC_EXIT_COMM     (65)
#define MUS_COMM_SYNC_START_SEND_SCRIPT (66)
#define MUS_COMM_TIMMING_DRESSUP_WAIT (67)
#define MUS_COMM_TIMMING_START_ACTING (68)
#define MUS_COMM_TIMMING_WAIT_FITTING (69)
#define MUS_COMM_TIMMING_INIT_COMM (70)
#define MUS_COMM_TIMMING_SHOT_CONFIRM (71)

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

//�J�n���Ɍ��������G�f�[�^(MYSTATUS��PPP���p�b�N����
typedef struct
{
  u16 sumPoint; //���v���_(�t�@���v�Z�Ɏg�p
  u16 pad;
}MUS_COMM_MISC_DATA;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

extern void MUS_COMM_InitMusical( MUS_COMM_WORK* work , MYSTATUS *myStatus , POKEMON_PASO_PARAM *ppp , GAME_COMM_SYS_PTR gameComm , MUSICAL_DISTRIBUTE_DATA *distData , const HEAPID heapId );
extern void MUS_COMM_ExitMusical( MUS_COMM_WORK* work );

extern MUS_COMM_WORK* MUS_COMM_CreateWork( HEAPID heapId , GAME_COMM_SYS_PTR gameComm , SAVE_CONTROL_WORK *saveCtrl , MUSICAL_DISTRIBUTE_DATA *distData );
extern void MUS_COMM_DeleteWork( MUS_COMM_WORK* work );

extern void MUS_COMM_InitComm( MUS_COMM_WORK* work );
extern void MUS_COMM_ExitComm( MUS_COMM_WORK* work );
extern void MUS_COMM_UpdateComm( MUS_COMM_WORK* work );
extern const BOOL MUS_COMM_IsInitComm( MUS_COMM_WORK* work );

extern void MUS_COMM_StartParent( MUS_COMM_WORK* work );
extern void MUS_COMM_StartChild( MUS_COMM_WORK* work );
extern const BOOL MUS_COMM_StartGame( MUS_COMM_WORK* work );
extern const BOOL MUS_COMM_IsStartGame( MUS_COMM_WORK* work );

//�Q�[����Ԃ̐ݒ�
extern const BOOL MUS_COMM_SetCommGameState( MUS_COMM_WORK *work , MUS_COMM_GAME_STATE state );

extern const BOOL MUS_COMM_Send_MusPokeData( MUS_COMM_WORK *work , MUSICAL_POKE_PARAM *musPoke );
extern const BOOL MUS_COMM_Send_AllMusPokeData( MUS_COMM_WORK *work  );
extern const BOOL MUS_COMM_CheckAllPostPokeData( MUS_COMM_WORK *work );

extern void MUS_COMM_Start_SendStrmData( MUS_COMM_WORK *work );
extern const BOOL MUS_COMM_Send_StrmData( MUS_COMM_WORK *work , const u8 idx );
extern const BOOL MUS_COMM_CheckFinishSendStrm( MUS_COMM_WORK *work );

extern const BOOL MUS_COMM_Send_ProgramSize( MUS_COMM_WORK *work );
extern const BOOL MUS_COMM_Send_ProgramData( MUS_COMM_WORK *work );
extern const BOOL MUS_COMM_Send_MessageSize( MUS_COMM_WORK *work );
extern const BOOL MUS_COMM_Send_MessageData( MUS_COMM_WORK *work );
extern const BOOL MUS_COMM_Send_ScriptSize( MUS_COMM_WORK *work );
extern const BOOL MUS_COMM_Send_ScriptData( MUS_COMM_WORK *work );

//�~���[�W�J���̖���IDX�Ńf�[�^���擾
extern MUSICAL_POKE_PARAM* MUS_COMM_GetMusPokeParam( MUS_COMM_WORK *work , const u8 musicalIdx );

extern void MUS_COMM_SendTimingCommand( MUS_COMM_WORK *work , const u8 no );
extern const BOOL MUS_COMM_CheckTimingCommand( MUS_COMM_WORK *work , const u8 no );

extern MYSTATUS *MUS_COMM_GetPlayerMyStatus( MUS_COMM_WORK* work , u8 idx );
extern POKEMON_PASO_PARAM* MUS_COMM_GetPlayerPPP( MUS_COMM_WORK* work , u8 idx );
extern const BOOL MUS_COMM_IsRefreshUserData( MUS_COMM_WORK *work );
extern void MUS_COMM_ResetRefreshUserData( MUS_COMM_WORK *work );

extern const BOOL MUS_COMM_Send_MusicalIndex( MUS_COMM_WORK* work );
extern const BOOL MUS_COMM_Send_UseButtonFlg( MUS_COMM_WORK* work , u8 pos );

extern u8 MUS_COMM_GetSelfMusicalIndex( MUS_COMM_WORK* work );
extern u8 MUS_COMM_GetMusicalIndex( MUS_COMM_WORK* work , const u8 idx);
extern const MUS_COMM_MISC_DATA* MUS_COMM_GetUserMiscData( MUS_COMM_WORK* work , const u8 idx );
extern GAME_COMM_SYS_PTR MUS_COMM_GetGameComm( MUS_COMM_WORK* work );
extern MUS_COMM_MODE MUS_COMM_GetMode( MUS_COMM_WORK* work );
extern const BOOL MUS_COMM_IsPostAllMyStatus( MUS_COMM_WORK* work );
extern const BOOL MUS_COMM_IsPostAllPoke( MUS_COMM_WORK* work );
extern void MUS_COMM_StartSendProgram_Data( MUS_COMM_WORK* work , u32 conArr , u32 npcArr );
extern void MUS_COMM_StartSendProgram_Script( MUS_COMM_WORK* work );
extern void MUS_COMM_StartSendPoke( MUS_COMM_WORK* work , MUSICAL_POKE_PARAM *musPoke);
extern u32 MUS_COMM_GetConditionPointArr( MUS_COMM_WORK* work );
extern u32 MUS_COMM_GetNpcArr( MUS_COMM_WORK* work );
extern const u8 MUS_COMM_GetAppealBonus( MUS_COMM_WORK* work , const u8 pokeIdx , const u8 dataIdx );

extern u8 MUS_COMM_GetUseButtonPos( MUS_COMM_WORK* work , const u8 musIdx );
extern void MUS_COMM_ResetUseButtonPos( MUS_COMM_WORK* work , const u8 musIdx );
extern u8 MUS_COMM_GetUseButtonAttention( MUS_COMM_WORK* work );
extern void MUS_COMM_ResetUseButtonAttention( MUS_COMM_WORK* work );
extern void MUS_COMM_ReqSendAppealBonusPoke( MUS_COMM_WORK* work , const u8 idx , const u8 pos , const u8 seType);
//NPC�p�O�b�Y�d�l���N�G�X�g
extern void MUS_COMM_SetReqUseItem_NPC( MUS_COMM_WORK* work , const u8 musIdx , const u8 pos );

extern GFLNetInitializeStruct* MUS_COMM_GetNetInitStruct(void);
