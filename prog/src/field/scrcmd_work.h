//======================================================================
/**
 * @file	scrcmd_work.h
 * @brief	�X�N���v�g�R�}���h�p���[�N
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"

#include "msgdata.h"
#include "print/wordset.h"

#include "field/fldmmdl.h"
#include "field/script.h"
#include "field/field_msgbg.h"
#include "field/fieldmap_proc.h"


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
  u16 zone_id;
  u16 script_id;
	SCRIPT_WORK *script;
	GAMESYS_WORK *gsys;
	GAMEDATA *gdata;
	MMDLSYS *mmdlsys;
	
  FLDMSGBG *fldMsgBG;
  FIELDMAP_WORK *fieldMap;
}SCRCMD_WORK_HEADER;

//--------------------------------------------------------------
/// SCRCMD_WORK_BALLOONWIN�@�o���[���E�B���h�E�֘A���[�N
//--------------------------------------------------------------
typedef struct
{
  u16 obj_id;
  u16 up_down;
  VecFx32 tail_pos;
}SCRCMD_BALLOONWIN_WORK;

//======================================================================
//	proto
//======================================================================
//�������A�폜
extern SCRCMD_WORK * SCRCMD_WORK_Create(
	const SCRCMD_WORK_HEADER *head, HEAPID heapID, HEAPID temp_heapID );
extern void SCRCMD_WORK_Delete( SCRCMD_WORK *work );

//�Q�ƁA�擾
extern HEAPID SCRCMD_WORK_GetHeapID( SCRCMD_WORK *work );
//extern HEAPID SCRCMD_WORK_GetTempHeapID( SCRCMD_WORK *work );
extern u16 SCRCMD_WORK_GetZoneID( SCRCMD_WORK *work );
extern u16 SCRCMD_WORK_GetScriptID( SCRCMD_WORK *work );
extern GAMESYS_WORK * SCRCMD_WORK_GetGameSysWork( SCRCMD_WORK *work );
extern GAMEDATA *SCRCMD_WORK_GetGameData( SCRCMD_WORK *work );
extern MMDLSYS * SCRCMD_WORK_GetMMdlSys( SCRCMD_WORK *work );
extern SCRIPT_WORK * SCRCMD_WORK_GetScriptWork( SCRCMD_WORK *work );
extern FLDMSGBG * SCRCMD_WORK_GetFldMsgBG( SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetMsgData(SCRCMD_WORK *work, GFL_MSGDATA *msgData);
extern GFL_MSGDATA * SCRCMD_WORK_GetMsgData( SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetMsgWinPtr( SCRCMD_WORK *work, void *msgWin );
extern void * SCRCMD_WORK_GetMsgWinPtr( SCRCMD_WORK *work );

extern SCRCMD_BALLOONWIN_WORK * SCRCMD_WORK_GetBalloonWinWork(
    SCRCMD_WORK *work );

extern void SCRCMD_WORK_SetCallProcTCB( SCRCMD_WORK *work, GFL_TCB *tcb );
extern GFL_TCB * SCRCMD_WORK_GetCallProcTCB( SCRCMD_WORK *work );

//�A�j���[�V�����R�}���h
extern void SCRCMD_WORK_SetMMdlAnmTCB( SCRCMD_WORK *work, GFL_TCB *tcb );
extern BOOL SCRCMD_WORK_CheckMMdlAnmTCB( SCRCMD_WORK *work );

//���b�Z�[�W�f�[�^
extern void SCRCMD_WORK_CreateMsgData( SCRCMD_WORK *work, u32 datID );

//���j���[
extern void SCRCMD_WORK_InitMenuWork( SCRCMD_WORK *work,
  u16 x, u16 y, u16 cursor, u16 cancel, u16 *ret,
  WORDSET *wordset, GFL_MSGDATA *msgData );
extern void SCRCMD_WORK_AddMenuList(
    SCRCMD_WORK *work, u32 msg_id, u32 param,
    STRBUF *msgbuf, STRBUF *tmpbuf );
extern void SCRCMD_WORK_StartMenu( SCRCMD_WORK *work );
extern BOOL SCRCMD_WORK_ProcMenu( SCRCMD_WORK *work );
