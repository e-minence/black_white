/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file	 scrcmd_gate.h
 * @brief	 �X�N���v�g�R�}���h�F�Q�[�g�֘A
 * @date   2009.11.05
 * @author obata
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once

//=======================================================================================
//  extern 
//=======================================================================================

#if 0  // �j���[�X�̊Ǘ����@��ύX��, �s�v�ɂȂ������߂ɍ폜 2009.12.17
extern VMCMD_RESULT EvCmdElboard_SetNormalNews( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdElboard_AddSpecialNews( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdElboard_Recovery( VMHANDLE *core, void *wk );
#endif

extern VMCMD_RESULT EvCmdLookElboard( VMHANDLE *core, void *wk );
