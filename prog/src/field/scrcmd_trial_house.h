//======================================================================
/**
 * @file  scrcmd_trial_house.h
 * @brief  �X�N���v�g�R�}���h�F�g���C�A���n�E�X
 * @author  Saito
 */
//======================================================================
#pragma once

//======================================================================
//  extern 
//======================================================================
extern VMCMD_RESULT EvCmdTH_Start( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTH_End( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTH_SetPlayMode( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTH_SelectPoke( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTH_SetTrainer( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTH_DispBeforeMsg( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTH_CallBattle( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTH_SetDLFlg( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTH_SetRank( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdTH_GetRank( VMHANDLE *core, void *wk );
