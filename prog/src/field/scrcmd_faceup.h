//======================================================================
/**
 * @file  scrcmd_faceup.h
 * @brief  �X�N���v�g�R�}���h�F��A�b�v�֘A
 * @author  Saito
 */
//======================================================================
#pragma once

extern VMCMD_RESULT EvCmdFaceup_Start( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdFaceup_End( VMHANDLE *core, void *wk );

extern BOOL SCREND_CheckEndFaceup(SCREND_CHECK *end_check , int *seq);

