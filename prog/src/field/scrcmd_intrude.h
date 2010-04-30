//==============================================================================
/**
 * @file    scrcmd_intrude.h
 * @brief   �X�N���v�g�R�}���h�F�N��
 * @author  matsuda
 * @date    2009.10.26(��)
 */
//==============================================================================
#ifndef __SCRCMD_INTRUDE_H__
#define __SCRCMD_INTRUDE_H__

//==============================================================================
//  �O���֐��錾
//==============================================================================
#ifndef	__ASM_NO_DEF_ //�A�Z���u���\�[�X�͖�����

extern VMCMD_RESULT EvCmdIntrudePalaceMmdlSetting( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdIntrudeConnectMapSetting( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
//  fieldmap�I�[�o�[���C�ɔz�u
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdIntrudeMissionChoiceListReq( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGetDisguiseCode( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGetMissionEntry( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdIntrudeMissionStartWait( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdIntrudeGetPalaceAreaOffset( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdIntrudeCheckEqpGPower( VMHANDLE *core, void *wk );

//-- �C�x���g�P�̔����o��
extern GMEVENT * EVENT_Intrude_MissionStartWait(GAMESYS_WORK * gsys);
extern GMEVENT * EVENT_Intrude_MissionStartWait_Warp(GAMESYS_WORK * gsys);


#endif	/* __ASM_NO_DEF_ */

#endif	/* __SCRCMD_INTRUDE_H__*/
