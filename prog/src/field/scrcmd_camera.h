//======================================================================
/**
 * @file  scrcmd_camera.h
 * @brief  �J��������X�N���v�g�R�}���h�p�֐�
 * @author  Nozomu Satio
 *
 */
//======================================================================

#pragma once

//======================================================================
//  extern 
//======================================================================
extern VMCMD_RESULT EvCmdCamera_Start( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdCamera_End( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdCamera_Purge( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdCamera_Bind( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdCamera_Move( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdCamera_RecoverMove( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdCamera_WaitMove( VMHANDLE *core, void *wk );

