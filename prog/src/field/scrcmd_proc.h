//======================================================================
/**
 * @file    scrcmd_proc.h
 * @brief   �X�N���v�g�R�}���h�p�֐��@�e��A�v���Ăяo���n(�풓)
 * @author  Miyuki Iwasawa 
 * @date    09.10.22
 *
 * �t�F�[�h���t�B�[���h����������܂ł��R�����̃C�x���g�V�[�P���X
 * (EVENT_FieldSubProc,EVENT_FieldSubProc_Callback�Ȃ�)�ɔC����ꍇ�A�v���Z�X�Ăяo���R�}���h���̂�
 * �풓�ɔz�u����K�v���Ȃ��̂ŁAscrcmd_proc_fld.c(�t�B�[���h�풓)�ɒu���Ă�������
 *
 * �t�F�[�h��t�B�[���h�v���Z�X�R���g���[�������O�ōs���v���Z�X�Ăяo���@�����K�v������ꍇ
 * ���̃\�[�X�ɂ����Ă��������B���ɕK�v���Ȃ���΁A�풓�������ߖ�̂��߁AEVENT_FieldSubProc��
 * �p�ӂ��ꂽ�v���Z�X�Ăяo���C�x���g�𗘗p���Ă�������
 */
//======================================================================

#pragma once

//�T�u�v���Z�X�Ăяo���p���[�N
typedef struct _CALL_PROC_WORK_TAG CALL_PROC_WORK;

struct _CALL_PROC_WORK_TAG{
  void* proc_work; //�v���Z�X�p���[�N
  void* cb_work; //�R�[���o�b�N�p���[�N
  void (*cb_func)(CALL_PROC_WORK* cpw); 
};

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

/*
 *  @brief  �T�u�v���Z�X�R�[����Wait�֐��̃Z�b�g�A�b�v(����������o�[�W����)
 *
 *  @param  core
 *  @param  work
 *  @param  ov_id         �T�u�v���Z�X�̃I�[�o�[���CID
 *  @param  proc_data     �T�u�v���Z�X�̊֐��w��
 *  @param  proc_work     �T�u�v���Z�X�p���[�N�BNULL�w���
 *  @param  callback      �T�u�v���Z�X�I�����ɌĂяo���R�[���o�b�N�֐��|�C���^�BNULL�w���
 *  @param  callback_work �ėp���[�N�BNULL�w���
 *
 *  ���Fcallback��NULL���w�肵���ꍇ�A�T�u�v���Z�X�I������
 *  proc_work��callback_work�ɑ΂��� GFL_HEAP_FreeMemory()���Ăяo���܂��B
 *  callback���w�肵���ꍇ�́Acallback���Ŗ����I�ɉ�������Ă�������
 */
extern void EVFUNC_CallSubProc( VMHANDLE *core, SCRCMD_WORK *work,
    FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void* proc_work,
    void (*cb_func)(CALL_PROC_WORK* cpw), void* callback_work );

//--------------------------------------------------------------
/**
 * @brief �T�u�v���Z�X�̏I����҂��܂�
 * @param   core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  TRUE    �I��
 * @retval  FALSE   �҂�
 *
 * ���g�p�͋ʓc����̋����ł��B
 * �@�v���Z�X�R�[���͌����Ƃ��āA�Ăяo���`�߂�l�擾�܂ł�One�R�}���h�Ŏ�������݌v�ɂ��邱�ƁI
 *   EVFUNC_CallSubProc()���g���R�[���o�b�N�t���ŃT�u�v���Z�X���Ăяo���܂�
 *
 * ��������ő҂����ꍇ�A�����I�� EvCmdFreeSubProcWork( VMHANDLE *core, void *wk ) ���g���ĉ�����Ă�������
 */
//--------------------------------------------------------------
extern BOOL EVFUNC_WaitSubProcEndNonFree( VMHANDLE *core, void *wk );

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
 * @brief   �o�b�O�v���Z�X���Ăяo���A�߂�l���擾���܂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdCallBagProc( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
/**
 * @brief   ���[���{�b�N�X��ʃv���Z�X���Ăяo���܂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdCallMailBoxProc( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdCallWazaRemindProc( VMHANDLE *core, void *wk );

//--------------------------------------------------------------
/**
 * @brief   ���m���X��ʃv���Z�X���Ăяo���܂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdCallMonolithProc( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdDemoScene( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdCallTVTDemo( VMHANDLE *core, void *wk );

