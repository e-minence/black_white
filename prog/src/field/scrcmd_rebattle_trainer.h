//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_rebattle_trainer.h
 *	@brief  �Đ�g���[�i�[�p�X�N���v�g
 *	@author	tomoya takahashi
 *	@date		2010.03.19
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

extern VMCMD_RESULT EvCmdReBattleTrainerStart( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdReBattleTrainerEnd( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdReBattleTrainerSetUpTrID( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdReBattleTrainerSetUp2vs2TrID( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdReBattleTrainerSetUpRndTr( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdReBattleTrainerClearTrEventFlag( VMHANDLE *core, void *wk );

#ifdef _cplusplus
}	// extern "C"{
#endif



