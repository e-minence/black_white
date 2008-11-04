//============================================================================================
/**
 * @file	game_event.h
 * @brief	�C�x���g����p�w�b�_
 * @author	tamada
 * @date	2005.07.22
 */
//============================================================================================

#ifndef	__GAME_EVENT_H__
#define	__GAME_EVENT_H__

#include <gflib.h>
//#include "field_common.h"
//#include "fieldobj.h"
//#include "system/procsys.h"
#include "gamesystem/gamesystem.h"

//=============================================================================
//
//	�^��`
//
//=============================================================================
typedef enum {
	GMEVENT_RES_CONTINUE = 0,
	GMEVENT_RES_FINISH,
}GMEVENT_RESULT;
//-----------------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�C�x���g����֐��̌^��`
 */
//-----------------------------------------------------------------------------
typedef GMEVENT_RESULT (*GMEVENT_FUNC)(GMEVENT_CONTROL *, int *, void *);

//=============================================================================
//
//	�֐��O���Q��
//
//=============================================================================

extern BOOL GAMESYSTEM_EVENT_Main(GAMESYS_WORK * gsys);
extern BOOL GAMESYSTEM_EVENT_IsExists(GAMESYS_WORK * gsys);
extern GMEVENT_CONTROL * GAMESYSTEM_EVENT_Set(GAMESYS_WORK * repw, GMEVENT_FUNC event_func, u32 work_size);
extern void GMEVENT_Change(GMEVENT_CONTROL * event, GMEVENT_FUNC next_func, u32 work_size);
extern GMEVENT_CONTROL * GMEVENT_Call(GMEVENT_CONTROL * parent, GMEVENT_FUNC sub_func, u32 work_size);

extern GAMESYS_WORK * GMEVENT_GetGameSysWork(GMEVENT_CONTROL * event);
extern int * GMEVENT_GetSequenceWork(GMEVENT_CONTROL * event);
extern void * GMEVENT_GetEventWork(GMEVENT_CONTROL * event);


//extern BOOL FieldEvent_Cmd_WaitSubProcEnd(GAMESYS_WORK * gsys);
//extern void FieldEvent_Cmd_SetMapProc(GAMESYS_WORK * gsys);
//extern BOOL FieldEvent_Cmd_WaitMapProcStart(GAMESYS_WORK * gsys);

//extern void EventCmd_CallSubProc(GMEVENT_CONTROL * event, const GFL_PROC_DATA * proc_data, void * param);


#endif /* __GAME_EVENT_H__ */
