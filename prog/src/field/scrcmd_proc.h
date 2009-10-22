//======================================================================
/**
 * @file    scrcmd_proc.h
 * @brief   �X�N���v�g�R�}���h�p�֐��@�e��A�v���Ăяo���n(�풓)
 * @author  Miyuki Iwasawa 
 * @date    09.10.22
 */
//======================================================================

#pragma once

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�}�b�v�v���Z�X����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdFieldOpen( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�}�b�v�v���Z�X�j��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdFieldClose( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
/**
 * @brief �T�u�v���Z�X�̏I����҂��܂�
 * @param   core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  TRUE    �I��
 * @retval  FALSE   �҂�
 */
//--------------------------------------------------------------
extern BOOL SCMD_WaitSubProcEnd( VMHANDLE *core, void *wk );
extern BOOL SCMD_WaitSubProcEndNonFree( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
/**
 * @brief �T�u�v���Z�X���[�N���
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdFreeSubProcWork( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
/**
 * @brief   �o�b�O�v���Z�X���Ăяo���܂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 *
 * ���T�u�v���Z�X���[�N�̉���������s��Ȃ��̂ŁA���ʂ̎擾���
 * �@�����I�� EvCmdFreeSubProcWork()���Ăяo������
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdCallBagProc( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
/**
 * @brief   �o�b�O�v���Z�X�̃��^�[���R�[�h���擾���܂�
 * @param   core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 *
 * ��EvCmdFreeSubProcWork() �̑O�ɌĂяo������
 *
 * �o�b�O�v���Z�X�̏I�����[�h�ƑI���A�C�e��No��Ԃ��܂�
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdGetBagProcResult( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
/**
 * @brief   �{�b�N�X�v���Z�X���Ăяo���܂�
 * @param   core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdCallBoxProc( VMHANDLE *core, void *wk );
