//============================================================================================
/**
 * @file	scrcmd.h
 * @bfief	スクリプトコマンド用関数
 * @author	Satoshi Nohara
 * @date	05.08.08
 *
 * 05.04.26 Hiroyuki Nakamura
 */
//============================================================================================
#ifndef SCRCMD_H
#define SCRCMD_H


//============================================================================================
//	定義
//============================================================================================

//#define EVCMD_MAX	(565)		//scrcmd.c ScriptCmdTblの要素数


//============================================================================================
//	グローバル変数
//============================================================================================
extern const VM_CMD ScriptCmdTbl[];

extern const u32 ScriptCmdMax;


//============================================================================================
//	extern宣言
//============================================================================================
extern BOOL EvCmdWaitSubProcEnd( VM_MACHINE * core );
extern BOOL EvWaitSubProcAndFree( VM_MACHINE * core );


#endif	/* SCRCMD_H */


