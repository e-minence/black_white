//======================================================================
/**
 * @file	scrcmd_work.h
 * @brief	スクリプトコマンド用ワーク
 * @author	kagaya
 * @data	05.07.13
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
///	SCRCMD_WORK_HEADER スクリプトコマンドで参照するワークを集めたヘッダー
//--------------------------------------------------------------
typedef struct
{
	SCRIPT_WORK *script;
	GAMESYS_WORK *gsys;
	GAMEDATA *gdata;
	MMDLSYS *mmdlsys;
	
  FLDMSGBG *fldMsgBG;
  FIELDMAP_WORK *fieldMap;
}SCRCMD_WORK_HEADER;

//======================================================================
//	proto
//======================================================================
//初期化、削除
extern SCRCMD_WORK * SCRCMD_WORK_Create(
	const SCRCMD_WORK_HEADER *head, HEAPID heapID, HEAPID temp_heapID );
extern void SCRCMD_WORK_Delete( SCRCMD_WORK *work );

//参照、取得
extern HEAPID SCRCMD_WORK_GetHeapID( SCRCMD_WORK *work );
extern HEAPID SCRCMD_WORK_GetTempHeapID( SCRCMD_WORK *work );
extern GAMESYS_WORK * SCRCMD_WORK_GetGameSysWork( SCRCMD_WORK *work );
extern GAMEDATA *SCRCMD_WORK_GetGameData( SCRCMD_WORK *work );
extern MMDLSYS * SCRCMD_WORK_GetMMdlSys( SCRCMD_WORK *work );
extern SCRIPT_WORK * SCRCMD_WORK_GetScriptWork( SCRCMD_WORK *work );
extern FLDMSGBG * SCRCMD_WORK_GetFldMsgBG( SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetMsgData( SCRCMD_WORK *work, GFL_MSGDATA *msgData );
extern GFL_MSGDATA * SCRCMD_WORK_GetMsgData( SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetFldMsgWinStream( SCRCMD_WORK *work, FLDMSGWIN_STREAM *msgWin );
extern FLDMSGWIN_STREAM * SCRCMD_WORK_GetFldMsgWinStream( SCRCMD_WORK *work );

extern void SCRCMD_WORK_SetTalkMsgWinTailPos(
    SCRCMD_WORK *work, const VecFx32 *pos );
extern const VecFx32 * SCRCMD_WORK_GetTalkMsgWinTailPos( SCRCMD_WORK *work );

//アニメーションコマンド
extern void SCRCMD_WORK_SetMMdlAnmTCB( SCRCMD_WORK *work, GFL_TCB *tcb );
extern BOOL SCRCMD_WORK_CheckMMdlAnmTCB( SCRCMD_WORK *work );

//メッセージデータ
extern void SCRCMD_WORK_CreateMsgData( SCRCMD_WORK *work, u32 datID );

//メニュー
extern void SCRCMD_WORK_InitMenuWork( SCRCMD_WORK *work,
  u16 x, u16 y, u16 cursor, u16 cancel, u16 *ret,
  WORDSET *wordset, GFL_MSGDATA *msgData );
extern void SCRCMD_WORK_AddMenuList(
    SCRCMD_WORK *work, u32 msg_id, u32 param,
    STRBUF *msgbuf, STRBUF *tmpbuf );
extern void SCRCMD_WORK_StartMenu( SCRCMD_WORK *work );
extern BOOL SCRCMD_WORK_ProcMenu( SCRCMD_WORK *work );
