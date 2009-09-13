//======================================================================
/**
 * @file	scrcmd_menuwin.h
 * @brief	�X�N���v�g�R�}���h�F���j���[�A��b�E�B���h�E�֘A
 * @date  2009.09.13
 * @author	tamada GAMEFREAK inc.
 *
 * 2009.09.13 scrcmd.c��蕪��������
 */
//======================================================================
#pragma once

//======================================================================
//  extern 
//======================================================================

extern VMCMD_RESULT EvCmdYesNoWin( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBmpMenuInit( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBmpMenuInitEx( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBmpMenuMakeList( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBmpMenuStart( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdTalkMsg( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTalkMsgAllPut( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTalkWinOpen( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTalkWinClose( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBalloonWinWrite( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBalloonWinTalkWrite( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBalloonWinClose( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTrainerMessageSet( VMHANDLE *core, void *wk );

