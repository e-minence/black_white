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
	GMEVENT_RES_CONTINUE = 0, ///<�C�x���g�p����
	GMEVENT_RES_FINISH,       ///<�C�x���g�I��
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

//-----------------------------------------------------------------------------
/*
 * @brief Overlay�C�x���g�R�[���@�C�x���g�����R�[���o�b�N�֐��^
 */
//-----------------------------------------------------------------------------
typedef GMEVENT* (*GMEVENT_CREATE_CB_FUNC)( GAMESYS_WORK* gsys, void* work );

//=============================================================================
//
//	�֐��O���Q��
//
//=============================================================================

extern BOOL GAMESYSTEM_EVENT_Main(GAMESYS_WORK * gsys);
extern BOOL GAMESYSTEM_EVENT_IsExists(const GAMESYS_WORK * gsys);
extern BOOL GAMESYSTEM_EVENT_CheckSet(GAMESYS_WORK * gsys, EVCHECK_FUNC ev_check, void * context);

//�{�̂�gamesystem/gamesystem.c�ɂ���̂Œ��ӁI
extern void GAMESYSTEM_EVENT_EntryCheckFunc(GAMESYS_WORK * gsys,
		EVCHECK_FUNC evcheck_func, void * context);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern GMEVENT_RESULT GMEVENT_Run(GMEVENT * event);
//------------------------------------------------------------------
//------------------------------------------------------------------
extern void GMEVENT_Delete(GMEVENT * event);

//=============================================================================
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief	�C�x���g����
 * @param	gsys
 * @param	parent		�e�C�x���g�ւ̃|�C���^
 * @param	event_func	�C�x���g����֐��ւ̃|�C���^
 * @param	work		�C�x���g����֐��̎g�p���郏�[�N�ւ̃|�C���^
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 *
 * @note
 * ����parent�͏펞NULL�������n���Ă���B
 * ����Ȃ��̂����c
 */
//------------------------------------------------------------------
extern GMEVENT * GMEVENT_Create(GAMESYS_WORK * gsys,
		GMEVENT * parent, GMEVENT_FUNC event_func, u32 work_size);


//------------------------------------------------------------------
//------------------------------------------------------------------
extern void GMEVENT_Change(GMEVENT * event, GMEVENT_FUNC next_func, u32 work_size);

//------------------------------------------------------------------
//  �C�x���g����C�x���g�ւ̐؂�ւ�
//------------------------------------------------------------------
extern void GMEVENT_ChangeEvent(GMEVENT * now_event, GMEVENT * next_event);

//------------------------------------------------------------------
//  �C�x���g����C�x���g�̌Ăяo��
//------------------------------------------------------------------
extern void GMEVENT_CallEvent(GMEVENT * parent, GMEVENT * child);

//------------------------------------------------------------------
//  �C�x���g����v���Z�X�̌Ăяo��
//------------------------------------------------------------------
extern void GMEVENT_CallProc( GMEVENT * parent, 
    FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void * pwk);

//------------------------------------------------------------------
/**
 * @brief Overlay�C�x���gCall���I�[�o�[���C���
 * @param ov_id       �Ăяo���C�x���g�̃I�[�o�[���CID�w��
 * @param cb_event_create_func  ������Call�������C�x���g�𐶐�����CallBack�w��
 * @param cb_work �C�x���g�����R�[���o�b�N�Ɉ����n�����[�N 
 * 
 * �����Ŏw��̃I�[�o�[���C�ɔz�u���ꂽ�C�x���g��Call���A
 * �I����ɃI�[�o�[���C������s���ďI������C�x���g�𐶐�����
 */
//------------------------------------------------------------------
extern GMEVENT* GMEVENT_CreateOverlayEventCall( GAMESYS_WORK* gsys, 
    FSOverlayID ov_id, GMEVENT_CREATE_CB_FUNC cb_event_create_func, void* work );

//=============================================================================
//=============================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
extern GMEVENT * GMEVENT_GetParentEvent(GMEVENT * event);
extern GAMESYS_WORK * GMEVENT_GetGameSysWork(GMEVENT * event);
extern int * GMEVENT_GetSequenceWork(GMEVENT * event);
extern void * GMEVENT_GetEventWork(GMEVENT * event);



#endif /* __GAME_EVENT_H__ */
