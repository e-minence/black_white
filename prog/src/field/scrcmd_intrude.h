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

extern VMCMD_RESULT EvCmdIntrudeMinimonoSetting( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdIntrudeMissionStart( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdIntrudeConnectMapSetting( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdSymbolPokeSet( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
//  fieldmap�I�[�o�[���C�ɔz�u
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdIntrudeMissionChoiceListReq( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGetDisguiseCode( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdPalaceInDisguise( VMHANDLE *core, void *wk );


#endif	/* __ASM_NO_DEF_ */

#endif	/* __SCRCMD_INTRUDE_H__*/
