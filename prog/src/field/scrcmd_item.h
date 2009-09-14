//======================================================================
/**
 * @file	scrcmd_item.h
 * @brief	�X�N���v�g�R�}���h�F�A�C�e���֘A
 * @author	Satoshi Nohara
 */
//======================================================================
#ifndef SCR_ITEM_H
#define SCR_ITEM_H

//======================================================================
//  extern 
//======================================================================
#ifndef	__ASM_NO_DEF_ //�A�Z���u���\�[�X�͖�����
extern VMCMD_RESULT EvCmdAddItem( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdSubItem( VMHANDLE *core, void *wk );			
extern VMCMD_RESULT EvCmdAddItemChk( VMHANDLE * core, void *wk );	
extern VMCMD_RESULT EvCmdCheckItem(VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdGetItemNum(VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdWazaMachineItemNoCheck(VMHANDLE * core, void *wk);
extern VMCMD_RESULT EvCmdGetPocketID(VMHANDLE * core, void *wk );

#endif	/* __ASM_NO_DEF_ */

#endif	/* SCR_ITEM_H */
