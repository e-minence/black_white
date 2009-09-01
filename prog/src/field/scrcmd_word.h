//======================================================================
/**
 * @file  scrcmd_word.c
 * @bfief  �X�N���v�g�R�}���h�F�P��֘A
 * @author	Satoshi Nohara
 */
//======================================================================
#ifndef SCR_WORD_H
#define SCR_WORD_H

//======================================================================
//  extern 
//======================================================================
#ifndef	__ASM_NO_DEF_ //�A�Z���u���\�[�X�͖�����
extern VMCMD_RESULT EvCmdPlayerName( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdItemName( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdItemWazaName( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdWazaName( VMHANDLE *core, void *wk );

#endif	/* __ASM_NO_DEF_ */

#endif	/* SCR_WORD_H */
