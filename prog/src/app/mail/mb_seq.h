//============================================================================================
/**
 * @file  mb_seq.h
 * @brief ���[���{�b�N�X��� ���C���V�[�P���X
 * @author  Akito Mori / Hiroyuki Nakamura
 * @date  09.12.03
 */
//============================================================================================
#ifndef __MB_SEQ_H__
#define __MB_SEQ_H__

//============================================================================================
//  �萔��`
//============================================================================================

// ���C���V�[�P���X��`
enum {
  MBSEQ_MAINSEQ_INIT = 0,       // ������
  MBSEQ_MAINSEQ_RELEASE,        // ���
  MBSEQ_MAINSEQ_WIPE,         // ���C�v
  MBSEQ_MAINSEQ_MSG_WAIT,       // ���b�Z�[�W�E�F�C�g
  MBSEQ_MAINSEQ_YESNO,        // �͂��E������
  MBSEQ_MAINSEQ_BUTTON_ANM,     // �{�^���A�j��
  MBSEQ_MAINSEQ_SUBPROC_CALL,     // �T�u�v���Z�X�Ăяo��
  MBSEQ_MAINSEQ_SUBPROC_MAIN,     // �T�u�v���Z�X���C��
  MBSEQ_MAINSEQ_START,        // ���[���{�b�N�X�J�n
  MBSEQ_MAINSEQ_MAIL_SELECT_MAIN,   // ���[���I�����C��
  MBSEQ_MAINSEQ_MAILBOX_END_SET,    // ���[���{�b�N�X�I������
  MBSEQ_MAINSEQ_MAIL_SELECT_PAGE_CHG, // ���[�����X�g�y�[�W�؂�ւ�
  MBSEQ_MAINSEQ_MAIL_MENU_MSG_SET,  // ���j���[���b�Z�[�W�Z�b�g
  MBSEQ_MAINSEQ_MAIL_MENU_SET,    // ���j���[�Z�b�g
  MBSEQ_MAINSEQ_MAIL_MENU_MAIN,   // ���j���[���C��
  MBSEQ_MAINSEQ_MAIL_READ_MAIN,   //�u���[������ށv���C��
  MBSEQ_MAINSEQ_MAIL_READ_END,    //�u���[������ށv�I��
  MBSEQ_MAINSEQ_MAILDEL_YESNO_SET,    //�u���[���������v�͂��E�������Z�b�g
  MBSEQ_MAINSEQ_MAILPOKESET_MSG,      // ���[������������H���b�Z�[�W�Z�b�g
  MBSEQ_MAINSEQ_MAILPOKESET_YESNO_SET,  //���[������������H�͂��E�������Z�b�g
  MBSEQ_MAINSEQ_ERASEMAIL_POKELIST_EXIT,    // �폜���[���Y�t�|�P�������X�g����̖߂��
  MBSEQ_MAINSEQ_ERASEMAIL_POKESET_CANCEL,   // �폜���[���|�P�Y�t�L�����Z��
  MBSEQ_MAINSEQ_ERASEMAIL_POKESET_CANCEL_END, // �폜���[���|�P�Y�t�L�����Z���I��
  MBSEQ_MAINSEQ_MAILWRITE_END,        // ���[���쐬��
  MBSEQ_MAINSEQ_MAILDEL_END,        //�u���[���������v�I��
  MBSEQ_MAINSEQ_END         // �I��

};


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ���C���V�[�P���X����
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param seq   ���C���V�[�P���X
 *
 * @retval  "TRUE = ������"
 * @retval  "FALSE = �I��"
 */
//--------------------------------------------------------------------------------------------
extern BOOL MBSEQ_Main( MAILBOX_SYS_WORK * syswk, int * seq );


#endif  // __MB_SEQ_H__
