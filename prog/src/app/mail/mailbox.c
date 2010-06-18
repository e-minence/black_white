//============================================================================================
/**
 * @file  mailbox.c
 * @brief ���[���{�b�N�X���
 * @author  Akito Mori / Hiroyuki Nakamura
 * @date  09.12.03
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "savedata/mail_util.h"
#include "mb_main.h"
#include "mb_seq.h"



// �v���Z�X��`�f�[�^
const GFL_PROC_DATA MailBoxProcData = {
  MailBoxProc_Init,
  MailBoxProc_Main,
  MailBoxProc_End,
};


#ifdef PM_DEBUG
static PMS_DATA debug_pms[3]={
  {1,1,{1,1} },{2,2,{2,2} },{2,2,{2,2} },
};
static const STRCODE mailtestname[16]={ 0x3084,0x3081, 0xffff };
// �L���ɂ���ƃe�X�g�f�[�^���Z�b�g����
//#define DEBUG_DATA_SET
#endif
//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F������
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT MailBoxProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  MAILBOX_SYS_WORK * syswk;

  OS_Printf( "�����������@���[���{�b�N�X�����J�n�@����������\n" );

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MAILBOX_SYS, 0x10000 );

  syswk = GFL_PROC_AllocWork( proc, sizeof(MAILBOX_SYS_WORK), HEAPID_MAILBOX_SYS );
  MI_CpuClearFast( syswk, sizeof(MAILBOX_SYS_WORK) );

  syswk->dat       = pwk;
  syswk->sv_mail   = GAMEDATA_GetMailBlock( syswk->dat->gamedata );
  syswk->next_seq  = MBSEQ_MAINSEQ_START;
  syswk->vintr_tcb = NULL;

  // ���[���{�b�N�X�V�X�e����PROCSYS�쐬
  syswk->mbProcSys = GFL_PROC_LOCAL_boot( HEAPID_MAILBOX_SYS );

  // �e�X�g�p�Ƀ��[���f�[�^��ǉ�
#ifdef DEBUG_DATA_SET
  {
    int i;
    MAIL_DATA *mail = MailData_CreateWork(HEAPID_MAILBOX_SYS);
    MailData_Clear(mail);
//    PMSDAT_Init( pms, msg->sentence_type );

    MailData_SetWriterName( mail, (STRCODE*)mailtestname);
    MailData_SetDesignNo( mail, 1 );
    PMSDAT_SetDebugRandomDeco(&debug_pms[0],HEAPID_MAILBOX_SYS);
    PMSDAT_SetDebug(&debug_pms[1]);
    PMSDAT_SetDebug(&debug_pms[2]);
    MailData_SetMsgByIndex( mail, &debug_pms[0], 0);
    MailData_SetMsgByIndex( mail, &debug_pms[1], 1);
    MailData_SetMsgByIndex( mail, &debug_pms[2], 2);

    if(MailData_IsEnable(mail)==FALSE){
      OS_Printf("���[���f�[�^�͖���\n");
    }

    // �e�X�g�f�[�^�ǉ�
    for(i=0;i<3;i++){
      MAIL_AddMailFormWork( syswk->sv_mail, MAILBLOCK_PASOCOM, i, mail );
    }

    GFL_HEAP_FreeMemory( mail );

    OS_Printf("mail num = %d\n",MAIL_GetEnableDataNum(syswk->sv_mail,MAILBLOCK_PASOCOM));
  }
#endif
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F���C��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT MailBoxProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  MAILBOX_SYS_WORK * syswk = mywk;

  if( MBSEQ_Main( syswk, seq ) == FALSE ){
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F�I��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT MailBoxProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  MAILBOX_SYS_WORK * syswk = mywk;

  // ���[���{�b�N�X�V�X�e����PROCSYS�I��
  GFL_PROC_LOCAL_Exit( syswk->mbProcSys );

  GFL_PROC_FreeWork( proc );

  GFL_HEAP_CheckHeapSafe( HEAPID_MAILBOX_SYS );
  GFL_HEAP_DeleteHeap( HEAPID_MAILBOX_SYS );

  OS_Printf("mailbox end return %d\n", syswk->dat->retMode);

  OS_Printf( "�����������@���[���{�b�N�X�����I���@����������\n" );

  return GFL_PROC_RES_FINISH;
}
