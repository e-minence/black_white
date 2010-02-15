//======================================================================
/**
 * @file  scrcmd_un.h
 * @brief  �X�N���v�g�R�}���h�F���A�֘A
 * @author  Saito
 */
//======================================================================
#pragma once

//======================================================================
//  extern 
//======================================================================
extern VMCMD_RESULT EvCmdUn_GetCountryNum( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdUn_GetValidDataNum( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdUn_SetTalkFlg( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdUn_CheckTalkFlg( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdUn_SetPlayerNature( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdUn_SetPlayerFavorite( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdUn_GetPlayerFavorite( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdUn_SetCountryInfo( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdUn_GetRoomObjCode( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdUn_GetRoomInfo( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdUn_GetRoomObjMsg( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdUn_CallFloorSelApp( VMHANDLE *core, void *wk );
