//======================================================================
/**
 * @file	scrcmd_sound.h
 * @brief	�X�N���v�g�R�}���h�F�T�E���h�֘A
 * @author	Satoshi Nohara
 */
//======================================================================
#ifndef SCR_SOUND_H
#define SCR_SOUND_H

//======================================================================
//  extern 
//======================================================================
#ifndef	__ASM_NO_DEF_ //�A�Z���u���\�[�X�͖�����

//BGM
extern VMCMD_RESULT EvCmdBgmPlay( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBgmStop( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBgmPlayerPause( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBgmPlayCheck( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBgmFadeOut( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBgmFadeIn( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBgmNowMapPlay( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdBgmFadeWait( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdPlayTempEventBGM( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdBgmWait( VMHANDLE *core, void *wk );

//SE
extern VMCMD_RESULT EvCmdSePlay( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdSeStop( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdSeWait(VMHANDLE *core, void *wk);

//ME
extern VMCMD_RESULT EvCmdMePlay(VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdMeWait(VMHANDLE *core, void *wk );

//����
extern VMCMD_RESULT EvCmdVoicePlay(VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdVoiceWait(VMHANDLE *core, void *wk );

// ISS
extern VMCMD_RESULT EvCmdIssSwitchOn( VMHANDLE* core, void* wk );
extern VMCMD_RESULT EvCmdIssSwitchOff( VMHANDLE* core, void* wk );
extern VMCMD_RESULT EvCmdIssSwitchCheck( VMHANDLE* core, void* wk );

#endif	/* __ASM_NO_DEF_ */

#endif	/* SCR_SOUND_H */
