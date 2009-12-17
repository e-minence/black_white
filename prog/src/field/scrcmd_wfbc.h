//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_wfbc.h
 *	@brief  WFBC�p�X�N���v�g�R�}���h�S
 *	@author	tomoya takahashi
 *	@date		2009.12.17
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


// WFBC�̉�b�J�n�E�I��
extern VMCMD_RESULT EvCmdWfbc_TalkStart( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdWfbc_TalkEnd( VMHANDLE *core, void *wk );

// �b���Ă���l�̃o�g���g���[�i�[ID�擾
extern VMCMD_RESULT EvCmdWfbc_GetBattleTrainerID( VMHANDLE *core, void *wk );

// �b������̊X�ɍs�������邩�H
extern VMCMD_RESULT EvCmdWfbc_IsTakesIt( VMHANDLE *core, void *wk );

// �b������̑O�̗��������邩�H
extern VMCMD_RESULT EvCmdWfbc_IsRireki( VMHANDLE *core, void *wk );




#ifdef _cplusplus
}	// extern "C"{
#endif



