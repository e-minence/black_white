/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file	 scrcmd_gate.h
 * @brief	 スクリプトコマンド：ゲート関連
 * @date   2009.11.05
 * @author obata
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once

//=======================================================================================
//  extern 
//=======================================================================================
extern VMCMD_RESULT EvCmdElboard_SetNormalNews( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdElboard_AddSpecialNews( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdElboard_Recovery( VMHANDLE *core, void *wk );
