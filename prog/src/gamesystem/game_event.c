//=============================================================================
/**
 * @file	game_event.c
 * @brief	�C�x���g����
 * @author	tamada
 * @date	2008.10.30	DP����ڐA�ioriginal 2005.07.22)
 */
//=============================================================================
#include <gflib.h>

#include "system/main.h"	//HEAPID�Q�Ƃ̂���

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

//=============================================================================
//=============================================================================
enum {
	HEAPID_LOCAL = HEAPID_TAMADA_DEBUG,
};
//-----------------------------------------------------------------------------
/**
 * �Q�[�����C�x���g�R���g���[���p���[�N��`
 */
//-----------------------------------------------------------------------------
struct _GMEVENT_CONTROL{
	GMEVENT * parent;	///<�e�i�Ăяo�����j�̃C�x���g�ւ̃|�C���^
	GMEVENT_FUNC	func;	///<����֐��ւ̃|�C���^
	int seq;				///<�V�[�P���X���[�N
	void * work;			///<����֐����ɌŗL���[�N�ւ̃|�C���^
	GAMESYS_WORK * gsys;	///<�t�B�[���h�S�̐��䃏�[�N�ւ̃|�C���^�i�Ȃ�ׂ��Q�Ƃ������Ȃ��j
};

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
 */
//------------------------------------------------------------------
GMEVENT * GMEVENT_Create(GAMESYS_WORK * gsys, GMEVENT * parent, GMEVENT_FUNC event_func, u32 work_size)
{
	GMEVENT * event;
	event = GFL_HEAP_AllocMemory(HEAPID_LOCAL, sizeof(GMEVENT));
	event->parent = parent;
	event->func = event_func;
	event->seq = 0;
	event->work = GFL_HEAP_AllocMemory(HEAPID_LOCAL, work_size);
	event->gsys = gsys;
	return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT_RESULT GMEVENT_Run(GMEVENT * event)
{
	return event->func(event, &event->seq, event->work);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GMEVENT_Delete(GMEVENT * event)
{
	if (event->work) {
		GFL_HEAP_FreeMemory(event->work);
	}
	GFL_HEAP_FreeMemory(event);
}

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�ؑ�
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 * @param	next_func	���̃C�x���g����֐��ւ̃|�C���^
 * @param	work_size	���̃C�x���g����֐��̎g�p���郏�[�N�̃T�C�Y
 */
//------------------------------------------------------------------
void GMEVENT_Change(GMEVENT * event, GMEVENT_FUNC next_func, u32 work_size)
{
	event->func = next_func;
	event->seq = 0;
	if (event->work) {
		GFL_HEAP_FreeMemory(event->work);
	}
	event->work = GFL_HEAP_AllocMemory(HEAPID_LOCAL, work_size);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void GMEVENT_ChangeEvent(GMEVENT * now_event, GMEVENT * next_event)
{
	GAMESYS_WORK * gsys = now_event->gsys;
	next_event->parent = now_event->parent;
	GMEVENT_Delete(now_event);
	GAMESYSTEM_SetEvent(gsys, next_event);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GMEVENT_CallEvent(GMEVENT * parent, GMEVENT * child)
{
	child->parent = parent;
	GAMESYSTEM_SetEvent(parent->gsys, child);
}

//=============================================================================
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief	�C�x���g����
 * @param	gsys
 * @retval	TRUE	�C�x���g�I��
 * @retval	FALSE	�C�x���g�p����
 */
//------------------------------------------------------------------
BOOL GAMESYSTEM_EVENT_Main(GAMESYS_WORK * gsys)
{
	GMEVENT * event = GAMESYSTEM_GetEvent(gsys);
	if (event == NULL) {
		return FALSE;
	}
	while (GMEVENT_Run(event) == GMEVENT_RES_FINISH) {
		GMEVENT * parent = GMEVENT_GetParentEvent(event);
		GMEVENT_Delete(event);
		GAMESYSTEM_SetEvent(gsys, parent);
		event = parent;
		if (parent == NULL) {
			return TRUE;
		}
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�N�����`�F�b�N
 * @param	gsys	�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @retval	TRUE	�C�x���g�N����
 * @retval	FALSE	�C�x���g�Ȃ�
 */
//------------------------------------------------------------------
BOOL GAMESYSTEM_EVENT_IsExists(GAMESYS_WORK * gsys)
{
	return (GAMESYSTEM_GetEvent(gsys) != NULL);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL GAMESYSTEM_EVENT_CheckSet(GAMESYS_WORK * gsys, EVCHECK_FUNC ev_check, void * context)
{
	GMEVENT * event;
	if (GAMESYSTEM_GetEvent(gsys) != NULL){
		return FALSE;
	}
	if (ev_check == NULL) {
		return FALSE;
	}
	event = ev_check(gsys, context);
	if (event != NULL) {
		GAMESYSTEM_SetEvent(gsys, event);
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
//
//	�C�x���g�R�}���h
//
//=============================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------

#if 0
//extern BOOL FieldEvent_Cmd_WaitSubProcEnd(GAMESYS_WORK * gsys);
//extern void FieldEvent_Cmd_SetMapProc(GAMESYS_WORK * gsys);
//extern BOOL FieldEvent_Cmd_WaitMapProcStart(GAMESYS_WORK * gsys);

//extern void EventCmd_CallSubProc(GMEVENT * event, const GFL_PROC_DATA * proc_data, void * param);

//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X����I���҂�
 * @param	gsys	�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @retval	TRUE	�T�u�v���Z�X���쒆
 * @retval	FALSE	�T�u�v���Z�X�I��
 */
//------------------------------------------------------------------
BOOL FieldEvent_Cmd_WaitSubProcEnd(GAMESYS_WORK * gsys)
{
	if (GameSystem_CheckFieldProcExists(gsys) || GameSystem_CheckSubProcExists(gsys)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FieldEvent_Cmd_SetMapProc(GAMESYS_WORK * gsys)
{
	GameSystem_CreateFieldProc(gsys);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FieldEvent_Cmd_WaitMapProcStart(GAMESYS_WORK * gsys)
{
	if (GameSystem_CheckFieldMain(gsys)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

//=============================================================================
//
//=============================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	int seq;
	const GFL_PROC_DATA * proc_data;
	void * param;
}EV_SUBPROC_WORK;
//------------------------------------------------------------------
/**
 * @brief	�T�u�C�x���g�F�T�u�v���Z�X�Ăяo��
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 * @retval	TRUE		�T�u�C�x���g�I��
 * @retval	FALSE		�T�u�C�x���g�p����
 */
//------------------------------------------------------------------
static BOOL GMEVENT_Sub_CallSubProc(GMEVENT * event)
{
	GAMESYS_WORK * gsys = GMEVENT_GetGameSysWork(event);
	EV_SUBPROC_WORK * esw = GMEVENT_GetSpecialWork(event);
	switch (esw->seq) {
	case 0:
		GameSystem_StartSubProc(gsys, esw->proc_data, esw->param);
		esw->seq ++;
		break;
	case 1:
		if (FieldEvent_Cmd_WaitSubProcEnd(gsys)) {
			break;
		}
		GFL_HEAP_FreeMemory(esw);
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------
/**
 * @brief	�C�x���g�[���R�}���h�F�T�u�v���Z�X�Ăяo��
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 * @param	proc_data	�v���Z�X��`�f�[�^�ւ̃|�C���^
 * @param	param		�p�����[�^�ւ̃|�C���^
 *
 * �T�u�v���Z�X���Ăяo���ďI����҂�
 */
//------------------------------------------------------------------
void EventCmd_CallSubProc(GMEVENT * event, const GFL_PROC_DATA * proc_data, void * param)
{
	EV_SUBPROC_WORK * esw = GFL_HEAP_AllocMemory(HEAPID_LOCAL, sizeof(EV_SUBPROC_WORK));
	esw->seq = 0;
	esw->proc_data = proc_data;
	esw->param = param;
	FieldEvent_Call(event, GMEVENT_Sub_CallSubProc, esw);
}
#endif

//=============================================================================
//
//
//
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief	�Q�[���V�X�e�����䃏�[�N�ւ̃|�C���^�擾
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 * @return	GAMESYS_WORK	�Q�[���V�X�e�����䃏�[�N�ւ̃|�C���^
 *
 * �Q�[���V�X�e�����䃏�[�N�̓|�P�����Q�[�����ł͍ŏ�ʂ̃��[�N�G���A�ƂȂ�B
 * ���̗̈�ɒ��ڎQ�Ƃ��邱�Ƃ́u�Q�[���V�X�e�����ł̃O���[�o���ϐ��ւ̃A�N�Z�X�v��
 * �������B�O���[�o���ϐ��ւ̃A�N�Z�X�͂Ȃ�ׂ�������ׂ��Ȃ̂ŁA���̊֐���
 * �Ȃ�ׂ��g�p���Ȃ��悤�ɂ������B
 *
 * �܂��̓v���O�����\�����T�O�I�ɐ؂蕪���邱�Ƃ�D�悷�邽�߁A
 * ��{�I�ɕK�v�ȕϐ��Ȃǂւ̃A�N�Z�X�͊֐��o�R�ɂ���B
 * ���̏�ŕs�v�Ȃ��͍̂폜���Ă����A���x�I�Ȗ�肪����ꍇ�̓|�C���^���ڎQ�Ƃ�
 * �l����B
 *
 */
//------------------------------------------------------------------
GAMESYS_WORK * GMEVENT_GetGameSysWork(GMEVENT * event)
{
	return event->gsys;
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�C�x���g�ʂ̃��[�N�ւ̃|�C���^
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 * @return	�e�C�x���g�p���[�N
 */
//------------------------------------------------------------------
void * GMEVENT_GetEventWork(GMEVENT * event)
{
	return event->work;
}

//------------------------------------------------------------------
/**
 * @brief	�V�[�P���X���[�N�̎擾
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 * @return	�V�[�P���X���[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
int * GMEVENT_GetSequenceWork(GMEVENT * event)
{
	return &event->seq;
}
//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
GMEVENT * GMEVENT_GetParentEvent(GMEVENT * event)
{
	return event->parent;
}

