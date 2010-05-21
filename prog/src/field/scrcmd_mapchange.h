//======================================================================
/**
 * @file	scrcmd_mapchange.h
 * @brief	�X�N���v�g�R�}���h�F�}�b�v�J�ڊ֘A
 * @date  2009.09.11
 * @author	tamada GAMEFREAK inc.
 */
//======================================================================
#pragma once

//======================================================================
//  extern 
//======================================================================
extern VMCMD_RESULT EvCmdMapChange( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdMapChangeBGMKeep( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdMapChangeBySandStream(VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdMapChangeByWarp( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdMapChangeToUnion( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdMapChangeNoFade( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdRailMapChange( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdRailMapChangeNoFade( VMHANDLE *core, void *wk );


