//======================================================================
/**
 * @file	scrcmd_fldmmdl.h
 * @brief	スクリプトコマンド：動作モデル関連
 * @date  2009.09.13
 * @author	tamada GAMEFREAK inc.
 *
 * 2009.09.13 scrcmd.cより分離させた
 */
//======================================================================
#pragma once

//======================================================================
//  extern 
//======================================================================


extern VMCMD_RESULT EvCmdObjAnime( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjAnimeWait( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdMoveCodeGet( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdMoveCodeChange( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjPosGet( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdPlayerPosGet( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdPlayerDirGet( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdObjAdd( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjDel( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjAddEvent( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjDelEvent( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjPosChange( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjEventFlagGet( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjParamGet( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjSetNotDelZoneChange( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTalkObjPauseAll( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjTurn( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGetObjID( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGetFrontObjID( VMHANDLE* core, void* wk );

extern VMCMD_RESULT SCRCMD_SUB_ObjPauseAll( VMHANDLE *core, SCRCMD_WORK *work );
extern void SCRCMD_SUB_PauseClearAll( SCRCMD_WORK *work );

extern VMCMD_RESULT EvCmdPlayerRequest( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdPlayerUpDownEffect( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdPlayerUpDownPos( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdPosPlayerTurn( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGetPlayerFrom( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdObjHighJump( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjWarpOut( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjFallIn( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdCheckPlayerIllegalOBJCode( VMHANDLE *core, void *wk );

//======================================================================
//  railマップ専用
//======================================================================
extern VMCMD_RESULT EvCmdPlayerRailLocationGet( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjRailLocationGet( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjRailSlipDown( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdPlayerRailLocationSet( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdObjRailLocationSet( VMHANDLE *core, void *wk );


