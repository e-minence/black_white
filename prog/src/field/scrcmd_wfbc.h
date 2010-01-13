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

// �b������̑O�̗��������邩�H
extern VMCMD_RESULT EvCmdWfbc_SetRirekiPlayerName( VMHANDLE *core, void *wk );


// �e�p�����[�^�̎擾
extern VMCMD_RESULT EvCmdWfbc_GetData( VMHANDLE *core, void *wk );

// BC NPC�����ڕW�l�@���Z
extern VMCMD_RESULT EvCmdWfbc_AddBCNpcWinTarget( VMHANDLE *core, void *wk );

// WF �ړI�|�P�����������Ă���̂��`�F�b�N
extern VMCMD_RESULT EvCmdWfbc_CheckWFTargetPokemon( VMHANDLE *core, void *wk );


#ifdef _cplusplus
}	// extern "C"{
#endif



