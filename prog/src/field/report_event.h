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

// ���[�N
typedef struct {
	// �O����n���Ă�������
	SAVE_CONTROL_WORK * sv;
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldWork;
  FLDMSGBG * msgBG;
  HEAPID	heapID;

	// �������Őݒ�
	GFL_TCBLSYS * tcbl;

  GFL_MSGDATA * msgData;
	GFL_BMPWIN * win;
	PRINT_STREAM * stream;
	STRBUF * strBuff;

	APP_TASKMENU_ITEMWORK	ynList[2];
	APP_TASKMENU_RES * ynRes;
	APP_TASKMENU_WORK * ynWork;

	TIMEICON_WORK * timeIcon;

}FMENU_REPORT_EVENT_WORK;


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ���C������
 *
 * @param		wk		���[�N
 * @param		seq		�V�[�P���X
 *
 * @retval	"TRUE = ������"
 * @retval	"FALSE = �I��"
 */
//--------------------------------------------------------------------------------------------
extern BOOL REPORTEVENT_Main( FMENU_REPORT_EVENT_WORK * wk, int * seq );
