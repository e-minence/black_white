//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_fourkings.h
 *	@brief  四天王スクリプトコマンド
 *	@author	tomoya takahashi
 *	@date		2009.11.25
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
// 四天王部屋　歩行デモ　開始
extern VMCMD_RESULT EvCmdFourKings_WalkEvent( VMHANDLE *core, void *wk );

// 四天王部屋 頂上　カメラ設定
extern VMCMD_RESULT EvCmdFourKings_SetCameraParam( VMHANDLE *core, void *wk );

// 四天王部屋　雷音システム　開始
extern VMCMD_RESULT EvCmdFourKings_SetGhostSparkSound( VMHANDLE *core, void *wk );

// 四天王部屋　サウンドシステム　開始
extern VMCMD_RESULT EvCmdFourKings_SetSoundSystem( VMHANDLE *core, void *wk );


#ifdef _cplusplus
}	// extern "C"{
#endif



