//======================================================================
/**
 * @file  scrcmd_gym.h
 * @brief  �X�N���v�g�R�}���h�F�W���֘A
 * @author  Saito
 */
//======================================================================
#ifndef __SCRCMD_GYM_H__
#define __SCRCMD_GYM_H__

//======================================================================
//  extern 
//======================================================================
#ifndef	__ASM_NO_DEF_ //�A�Z���u���\�[�X�͖�����
extern VMCMD_RESULT EvCmdGymElec_Init( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGymElec_PushSw( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdGymNormal_Init( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGymNormal_Unrock( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGymNormal_CheckUnrock( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGymNormal_MoveWall( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdGymAnti_Init( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGymAnti_SwOn( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGymAnti_SwOff( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGymAnti_OpenDoor( VMHANDLE *core, void *wk );

#endif	//__ASM_NO_DEF_

#endif    //__SCRCMD_GYM_H__

