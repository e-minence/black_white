//============================================================================================
/**
 * @file	game_event.h
 * @brief	�C�x���g����p�w�b�_
 * @author	tamada
 * @date	2008.10.30	DP����ڐA�ioriginal 2005.07.22)
 */
//============================================================================================

#ifndef	__GAME_EVENT_H__
#define	__GAME_EVENT_H__

#include <gflib.h>
#include "gamesystem/gamesystem.h"

//=============================================================================
//
//	�^��`
//
//=============================================================================
//-----------------------------------------------------------------------------
/**
 * @brief	�C�x���g����֐��̖߂�l��`
 */
//-----------------------------------------------------------------------------
typedef enum {
	GMEVENT_RES_CONTINUE = 0,
	GMEVENT_RES_FINISH,
}GMEVENT_RESULT;
//-----------------------------------------------------------------------------
/**
 * @brief	�C�x���g����֐��̌^��`
 */
//-----------------------------------------------------------------------------
typedef GMEVENT_RESULT (*GMEVENT_FUNC)(GMEVENT *, int *, void *);

//-----------------------------------------------------------------------------
/**
 * @brief	�C�x���g�`�F�b�N�֐��̌^��`
 */
//-----------------------------------------------------------------------------
typedef GMEVENT * (*EVCHECK_FUNC)(GAMESYS_WORK *, void*);

//=============================================================================
//
//	�֐��O���Q��
//
//=============================================================================

extern BOOL GAMESYSTEM_EVENT_Main(GAMESYS_WORK * gsys);
extern BOOL GAMESYSTEM_EVENT_IsExists(GAMESYS_WORK * gsys);
extern BOOL GAMESYSTEM_EVENT_CheckSet(GAMESYS_WORK * gsys, EVCHECK_FUNC ev_check, void * context);

//�{�̂�gamesystem/gamesystem.c�ɂ���̂Œ��ӁI
extern void GAMESYSTEM_EVENT_EntryCheckFunc(GAMESYS_WORK * gsys,
		EVCHECK_FUNC evcheck_func, void * context);

//------------------------------------------------------------------
/**
 * @brief	�C�x���g����
 * @param	gsys
 * @param	parent		�e�C�x���g�ւ̃|�C���^
 * @param	event_func	�C�x���g����֐��ւ̃|�C���^
 * @param	work		�C�x���g����֐��̎g�p���郏�[�N�ւ̃|�C���^
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 *
 */
//------------------------------------------------------------------
extern GMEVENT * GMEVENT_Create(GAMESYS_WORK * gsys,
		GMEVENT * parent, GMEVENT_FUNC event_func, u32 work_size);
//------------------------------------------------------------------
//------------------------------------------------------------------
extern GMEVENT_RESULT GMEVENT_Run(GMEVENT * event);
//------------------------------------------------------------------
//------------------------------------------------------------------
extern void GMEVENT_Delete(GMEVENT * event);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void GMEVENT_Change(GMEVENT * event, GMEVENT_FUNC next_func, u32 work_size);
extern void GMEVENT_ChangeEvent(GMEVENT * now_event, GMEVENT * next_event);
//------------------------------------------------------------------
//------------------------------------------------------------------
extern void GMEVENT_CallEvent(GMEVENT * parent, GMEVENT * child);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern GMEVENT * GMEVENT_GetParentEvent(GMEVENT * event);
extern GAMESYS_WORK * GMEVENT_GetGameSysWork(GMEVENT * event);
extern int * GMEVENT_GetSequenceWork(GMEVENT * event);
extern void * GMEVENT_GetEventWork(GMEVENT * event);



#endif /* __GAME_EVENT_H__ */
