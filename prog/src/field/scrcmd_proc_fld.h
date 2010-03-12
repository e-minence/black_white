//======================================================================
/**
 * @file    scrcmd_proc_fld.h
 * @brief   �X�N���v�g�R�}���h�p�֐��@�e��A�v���Ăяo���n(�t�B�[���h�풓)
 * @author  Miyuki Iwasawa 
 * @date    09.10.23
 *
 * �t�F�[�h��t�B�[���h�v���Z�X�R���g���[�������O�ōs���v���Z�X�Ăяo���@�����
 * �v���Z�X�Ăяo���R�}���h���̂��풓���Ă���K�v������Ƃ���
 * scrcmd_proc.c�ɃR�}���h��u���Ă�������
 */
//======================================================================

#pragma once

//--------------------------------------------------------------
/**
 * @brief   �{�b�N�X�v���Z�X���Ăяo���܂�
 * @param   core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdCallBoxProc( VMHANDLE *core, void *wk ); 

//--------------------------------------------------------------
/**
 * @brief   ����Ⴂ�w���A���b�Z�[�W�x���͉�ʂ̌Ăяo��
 * @param   core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCrossCommHelloMessageInputCall( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
//  �Q�[���N���A�����Ăяo��
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdGameClearDemo( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
/// C�M�A����f���Ăяo��
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdCallCGearGetDemo( VMHANDLE *core, void *wk );

///�W�I�l�b�g�Ăяo��
extern VMCMD_RESULT EvCmdCallGeonet( VMHANDLE *core, void *wk ); 
