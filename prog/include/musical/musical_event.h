//======================================================================
/**
 * @file  musical_event.c
 * @brief �~���[�W�J���Ǘ��C�x���g
 * @author  ariizumi
 * @data  09/09/08
 *
 * ���W���[�����FMUSICAL_
 */
//======================================================================
#pragma once

#include "musical_system.h"

typedef struct _MUSICAL_EVENT_WORK MUSICAL_EVENT_WORK;

extern GMEVENT* MUSICAL_CreateEvent( GAMESYS_WORK * gsys , GAMEDATA *gdata , const u8 pokeIdx , const BOOL isComm , MUSICAL_SCRIPT_WORK *scriptWork );

extern const u8 MUSICAL_EVENT_GetSelfIndex( MUSICAL_EVENT_WORK *evWork );
extern const u8 MUSICAL_EVENT_GetPosIndex( MUSICAL_EVENT_WORK *evWork , const u8 pos );
extern const u8 MUSICAL_EVENT_GetMaxCondition( MUSICAL_EVENT_WORK *evWork );
extern STRBUF* MUSICAL_EVENT_CreateStr_ProgramTitle( MUSICAL_EVENT_WORK *evWork , HEAPID heapId );
extern const u8 MUSICAL_EVENT_GetPoint( MUSICAL_EVENT_WORK *evWork , const u8 idx );
extern const u16 MUSICAL_EVENT_GetMaxPoint( MUSICAL_EVENT_WORK *evWork );
extern const u16 MUSICAL_EVENT_GetMinPoint( MUSICAL_EVENT_WORK *evWork );
extern const u8 MUSICAL_EVENT_GetPosToRank( MUSICAL_EVENT_WORK *evWork , const u8 idx );
extern const u8 MUSICAL_EVENT_GetMaxPointCondition( MUSICAL_EVENT_WORK *evWork , const u8 idx );
extern const u8 MUSICAL_EVENT_GetPosObjView( MUSICAL_EVENT_WORK *evWork , const u8 idx );
extern void MUSICAL_EVENT_SetPosCharaName_Wordset( MUSICAL_EVENT_WORK *evWork , const u8 pos , WORDSET *wordSet , const u8 wordIdx );
extern void MUSICAL_EVENT_SetPosPokeName_Wordset( MUSICAL_EVENT_WORK *evWork , const u8 pos , WORDSET *wordSet , const u8 wordIdx );
extern void MUSICAL_EVENT_CalcProgramData( MUSICAL_EVENT_WORK *evWork );
extern void MUSICAL_EVENT_CheckNetErr( MUSICAL_EVENT_WORK *evWork );
extern const BOOL MUSICAL_EVENT_IsNetErr( MUSICAL_EVENT_WORK *evWork );
