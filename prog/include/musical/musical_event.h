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

typedef struct _MUSICAL_EVENT_WORK MUSICAL_EVENT_WORK;

extern GMEVENT* MUSICAL_CreateEvent( GAMESYS_WORK * gsys , GAMEDATA *gdata , const BOOL isComm );
