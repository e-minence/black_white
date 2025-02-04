//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_item_judge.h
 *	@brief  鑑定士　スクリプトコマンド
 *	@author	tomoya takahashi
 *	@date		2010.03.19
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern VMCMD_RESULT EvCmdItemJudgeCheck( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdItemJudgeHaveCheck( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdItemJudgeSetWordSet( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdItemJudgeAddMoney( VMHANDLE *core, void *wk );

#ifdef _cplusplus
}	// extern "C"{
#endif



