//======================================================================
/**
 * @file	scrcmd_work.h
 * @brief	スクリプトコマンド用ワーク
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
//--------------------------------------------------------------
/// SCR_MENU_JUSTIFY メニュー左右詰め識別
//--------------------------------------------------------------
typedef enum
{
  SCR_MENU_JUST_L = 0, ///<左詰
  SCR_MENU_JUST_R, ///<右詰
  SCR_MENU_JUST_MAX, ///<最大
}SCR_MENU_JUSTIFY;

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
  u16 zone_id;
  u16 script_id;
  BOOL sp_flag;   ///<特殊スクリプトかどうかの判別ヘッダ
  
	SCRIPT_WORK *script_work;
	GAMESYS_WORK *gsys;
	GAMEDATA *gdata;
	MMDLSYS *mmdlsys;
	
  FLDMSGBG *fldMsgBG;
  FIELDMAP_WORK *fieldMap;
}SCRCMD_WORK_HEADER;

//--------------------------------------------------------------
/// SCRCMD_WORK_BALLOONWIN　バルーンウィンドウ関連ワーク
//--------------------------------------------------------------
typedef struct
{
  u16 obj_id;
  u16 win_idx;
  VecFx32 tail_pos;
  VecFx32 tail_pos_org;
  VecFx32 tail_offs;
}SCRCMD_BALLOONWIN_WORK;

//======================================================================
//	proto
//======================================================================
//初期化、削除
extern SCRCMD_WORK * SCRCMD_WORK_Create(
	const SCRCMD_WORK_HEADER *head, HEAPID heapID );
extern void SCRCMD_WORK_Delete( SCRCMD_WORK *work );

//参照、取得
extern HEAPID SCRCMD_WORK_GetHeapID( SCRCMD_WORK *work );
//extern HEAPID SCRCMD_WORK_GetTempHeapID( SCRCMD_WORK *work );
extern u16 SCRCMD_WORK_GetZoneID( SCRCMD_WORK *work );
extern u16 SCRCMD_WORK_GetScriptID( SCRCMD_WORK *work );
extern BOOL SCRCMD_WORK_GetSpScriptFlag( const SCRCMD_WORK *work );
extern GAMESYS_WORK * SCRCMD_WORK_GetGameSysWork( SCRCMD_WORK *work );
extern GAMEDATA *SCRCMD_WORK_GetGameData( SCRCMD_WORK *work );
extern MMDLSYS * SCRCMD_WORK_GetMMdlSys( SCRCMD_WORK *work );
extern SCRIPT_WORK * SCRCMD_WORK_GetScriptWork( SCRCMD_WORK *work );
extern FLDMSGBG * SCRCMD_WORK_GetFldMsgBG( SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetMsgData(SCRCMD_WORK *work, GFL_MSGDATA *msgData);
extern GFL_MSGDATA * SCRCMD_WORK_GetMsgData( SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetMsgWinPtr( SCRCMD_WORK *work, void *msgWin );
extern void * SCRCMD_WORK_GetMsgWinPtr( SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetTimeIconPtr( SCRCMD_WORK *work, void *timeIcon );
extern void * SCRCMD_WORK_GetTimeIconPtr( SCRCMD_WORK *work );

extern SCRCMD_BALLOONWIN_WORK * SCRCMD_WORK_GetBalloonWinWork(
    SCRCMD_WORK *work );

extern void SCRCMD_WORK_SetCallProcTCB( SCRCMD_WORK *work, GFL_TCB *tcb );
extern GFL_TCB * SCRCMD_WORK_GetCallProcTCB( SCRCMD_WORK *work );
extern u8 SCRCMD_WORK_GetBeforeWindowPosType( const SCRCMD_WORK *work );
extern void SCRCMD_WORK_SetBeforeWindowPosType( SCRCMD_WORK *work, u8 type );

//アニメーションコマンド
extern void SCRCMD_WORK_SetMMdlAnmTCB( SCRCMD_WORK *work, GFL_TCB *tcb );
extern BOOL SCRCMD_WORK_CheckMMdlAnmTCB( SCRCMD_WORK *work );

//メッセージデータ
extern void SCRCMD_WORK_CreateMsgData( SCRCMD_WORK *work, u32 datID );

//メニュー
extern void SCRCMD_WORK_InitMenuWork( SCRCMD_WORK *work,
  u16 x, u16 y, u16 cursor, u16 cancel, SCR_MENU_JUSTIFY lr_just, u16 *ret,
  WORDSET *wordset, GFL_MSGDATA *msgData );
extern void SCRCMD_WORK_AddMenuList(
    SCRCMD_WORK *work, u32 msg_id, u32 ex_msg_id, u32 param,
    STRBUF *msgbuf, STRBUF *tmpbuf );
extern void SCRCMD_WORK_StartMenu( SCRCMD_WORK *work );
extern BOOL SCRCMD_WORK_ProcMenu( SCRCMD_WORK *work );
extern BOOL SCRCMD_WORK_ProcMenu_Breakable( SCRCMD_WORK *work );

//退避用ワーク操作
extern void SCRCMD_WORK_BackupUserWork( SCRCMD_WORK *work );
extern void SCRCMD_WORK_RestoreUserWork( SCRCMD_WORK *work );

