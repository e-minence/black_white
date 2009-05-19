//======================================================================
/**
 * @file	scrcmd_work.h
 * @brief	�X�N���v�g�R�}���h�p���[�N
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#ifndef SCRCMD_WORK_H_FILE
#define SCRCMD_WORK_H_FILE

#include <gflib.h>
#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"

#include "field/fldmmdl.h"
#include "field/script.h"
#include "field/field_msgbg.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	SCRCMD_WORK
//--------------------------------------------------------------
typedef struct _TAG_SCRCMD_WORK SCRCMD_WORK;

//--------------------------------------------------------------
///	SCRCMD_WORK_HEADER �X�N���v�g�R�}���h�ŎQ�Ƃ��郏�[�N���W�߂��w�b�_�[
//--------------------------------------------------------------
typedef struct
{
	GAMESYS_WORK *gsys;
	GAMEDATA *gdata;
	SCRIPT_WORK *script;
	FLDMMDLSYS *fldmmdlsys;

	FLDMSGBG *fldMsgBG;
}SCRCMD_WORK_HEADER;

//======================================================================
//	proto
//======================================================================
//�������A�폜
extern SCRCMD_WORK * SCRCMD_WORK_Create(
	const SCRCMD_WORK_HEADER *head, HEAPID heapID );
extern void SCRCMD_WORK_Delete( SCRCMD_WORK *work );

//�Q�ƁA�擾
extern HEAPID SCRCMD_WORK_GetHeapID( SCRCMD_WORK *work );
extern GAMESYS_WORK * SCRCMD_WORK_GetGameSysWork( SCRCMD_WORK *work );
extern GAMEDATA *SCRCMD_WORK_GetGameData( SCRCMD_WORK *work );
extern FLDMMDLSYS * SCRCMD_WORK_GetFldMMdlSys( SCRCMD_WORK *work );
extern SCRIPT_WORK * SCRCMD_WORK_GetScriptWork( SCRCMD_WORK *work );
extern FLDMSGBG * SCRCMD_WORK_GetFldMsgBG( SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetMsgData( SCRCMD_WORK *work, GFL_MSGDATA *msgData );
extern GFL_MSGDATA * SCRCMD_WORK_GetMsgData( SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetFldMsgWinStream( SCRCMD_WORK *work, FLDMSGWIN_STREAM *msgWin );
extern FLDMSGWIN_STREAM * SCRCMD_WORK_GetFldMsgWinStream( SCRCMD_WORK *work );

//�A�j���[�V�����R�}���h
extern void SCRCMD_WORK_SetFldMMdlAnmTCB( SCRCMD_WORK *work, GFL_TCB *tcb );
extern BOOL SCRCMD_WORK_CheckFldMMdlAnmTCB( SCRCMD_WORK *work );

//���̑�
extern void SCRCMD_WORK_CreateMsgData( SCRCMD_WORK *work, u32 datID );
#endif //SCRCMD_WORK_H_FILE
