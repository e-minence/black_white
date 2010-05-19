//============================================================================================
/**
 * @file		report_event.h
 * @brief		�t�B�[���h���j���[�E���|�[�g����
 * @author	Hiroyuki Nakamura
 * @date		10.02.20
 */
//============================================================================================
#pragma	once

#include "gamesystem/gamesystem.h"
#include "system/time_icon.h"
#include "app/app_taskmenu.h"


//============================================================================================
//	�萔��`
//============================================================================================

typedef struct _REPORT_EVENT_LOCAL REPORT_EVENT_LOCAL;	// ���[�J�����[�N

// ���[�N
typedef struct {
	// �O����n���Ă�������
	SAVE_CONTROL_WORK * sv;
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldWork;
  FLDMSGBG * msgBG;
  HEAPID	heapID;
	void * resultAddr;

	REPORT_EVENT_LOCAL * local;		// ���[�J�����[�N�i�����ō쐬����܂��j

}FMENU_REPORT_EVENT_WORK;

enum {
	REPORTEVENT_RET_SAVE = 0,		// �Z�[�u���s
	REPORTEVENT_RET_CANCEL,			// �L�����Z��
	REPORTEVENT_RET_NONE,				// ������
};

//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C������
 *
 * @param		wk		���[�N
 * @param		seq		�V�[�P���X
 *
 * @retval	"REPORTEVENT_RET_SAVE = �Z�[�u���s"
 * @retval	"REPORTEVENT_RET_CANCEL = �L�����Z��"
 * @retval	"REPORTEVENT_RET_NONE = ������"
 */
//--------------------------------------------------------------------------------------------
extern int REPORTEVENT_Main( FMENU_REPORT_EVENT_WORK * wk, int * seq );
