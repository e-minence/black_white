/**
 *  @file mailsys.c
 *  @brief  ���[���t���[�R���g���[���t���[��
 *  @author Akito Mori / Miyuki Iwasawa
 *  @date 09.11.10
 */
 
#include <gflib.h>
#include <procsys.h>

#include "system/main.h"
#include "poke_tool/poke_tool.h"
#include "item/itemsym.h"
#include "item/item.h"
#include "sound/pm_sndsys.h"
#include "font/font.naix"
#include "gamesystem/game_data.h"
#include "app/mailtool.h"
#include "savedata/mail_util.h"
#include "savedata/config.h"
#include "mail_param.h"
#include "mailview.h"
#include "app/pms_input.h"
#include "mail_snd_def.h"
#include "msg/msg_pmss_system.h"

//�I�[�o�[���C�v���Z�X��`�f�[�^
const GFL_PROC_DATA MailSysProcData = {
    MailSysProc_Init,
    MailSysProc_Main,
    MailSysProc_End,
};


enum{
  MAILCASE_INIT,
  MAILCASE_WAIT,
  MAILCASE_END,
  WORDCASE_INIT,
  WORDCASE_WAIT,
  WORDCASE_END,
};

//================================================================
//�\���̐錾
//================================================================
//���[���V�X�e�����C�����[�N
typedef struct _MAIL_MAIN_DAT{
  int heapID;   ///<�q�[�vID
  u16 ret_val;  ///<���^�[���l
  u16 ret_no;   ///<���^�[���ҏWNO

  void  *app_wk;      ///<�ȈՉ�b���W���[�����[�N�̕ۑ�
  GFL_PROC *proc;     ///<�T�u�v���Z�X���[�N
  MAIL_TMP_DATA *dat; ///<���[���f�[�^�e���|����

  PMS_DATA  tmpPms;   //<�ȈՉ�b�e���|����

  GFL_PROCSYS *localProcSys;

}MAIL_MAIN_DAT;

//================================================================
//�v���g�^�C�v�錾�@���[�J��
//================================================================
static MAIL_TMP_DATA* MailSys_AllocTmpData(const MAIL_DATA* org,int heapID);
static void MailSys_ReleaseTmpData(MAIL_TMP_DATA* tmp);
static void MailSys_SetTmpData(MAIL_DATA* org,MAIL_TMP_DATA* tmp);
static void _set_pmsinput_mode( MAIL_MAIN_DAT* wk, MAIL_PARAM* param );


//=============================================================================================
/**
 * @brief ���[���쐬�Ăяo���@������
 *
 * @param   proc    
 * @param   seq   
 * @param   pwk   
 * @param   mywk    
 *
 * @retval  GFL_PROC_RESULT   
 */
//=============================================================================================
GFL_PROC_RESULT MailSysProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  MAIL_MAIN_DAT* wk;
  MAIL_PARAM* param = pwk;
  

  //�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MAILSYS,0x1000 );
  wk = GFL_PROC_AllocWork( proc, sizeof(MAIL_MAIN_DAT), HEAPID_MAILSYS);
  MI_CpuClearFast(wk,sizeof(MAIL_MAIN_DAT));

  //�q�[�vID�ۑ�
  wk->heapID = HEAPID_MAILSYS;

  //�f�[�^�e���|�����쐬
  wk->dat = MailSys_AllocTmpData( param->pDat, wk->heapID );

  if(param->mode == MAIL_MODE_CREATE){
    //�����i���o�[���i�[����Ă���̂ŁA��ʃ��[�N����擾����
    wk->dat->design = param->designNo; //�f�U�C��No�K�p
  }
  
  if(wk->dat->design >= MAIL_DESIGN_MAX){
    wk->dat->design = 0;
  }
  wk->dat->val = param->mode;

  // ���[���V�X�e����PROCSYS�쐬
  wk->localProcSys = GFL_PROC_LOCAL_boot( HEAPID_MAILSYS );
  
  //���݂̃L�[���샂�[�h���擾
//  wk->dat->kt_status = param->pKeytouch;

  return GFL_PROC_RES_FINISH;
}

//=============================================================================================
/**
 * @brief ���[���쐬���C��
 *
 * @param   proc    
 * @param   seq   
 * @param   pwk   
 * @param   mywk    
 *
 * @retval  GFL_PROC_RESULT   
 */
//=============================================================================================
GFL_PROC_RESULT MailSysProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  MAIL_MAIN_DAT* wk = mywk;
  MAIL_PARAM* param = pwk;

  // �I�[�o�[���CID�錾
  FS_EXTERN_OVERLAY(pmsinput);
  FS_EXTERN_OVERLAY(ui_common);
  
  // �v���Z�X��`�f�[�^
  static const GFL_PROC_DATA MailViewProcData = {
    MailViewProc_Init,
    MailViewProc_Main,
    MailViewProc_End,
  };
  static const GFL_PROC_DATA PMSProcData = {
    PMSInput_Init,
    PMSInput_Main,
    PMSInput_Quit,
  };

  //���[�J��PROC�̍X�V����
  GFL_PROC_MAIN_STATUS proc_status = GFL_PROC_LOCAL_Main( wk->localProcSys );


  switch(*seq){
  case MAILCASE_INIT:
    wk->dat->val = param->mode; //���샂�[�h�l���A
    GFL_PROC_LOCAL_CallProc( wk->localProcSys, NO_OVERLAY_ID, &MailViewProcData, wk->dat );
    *seq = MAILCASE_WAIT;
    break;
  case MAILCASE_WAIT:
    if(proc_status != GFL_PROC_MAIN_VALID){
      switch(wk->dat->val){
      case VIEW_END_CANCEL:
        *seq = MAILCASE_END;
        break;
      case VIEW_END_DECIDE:
        *seq = MAILCASE_END;
        break;
      default:
        *seq = WORDCASE_INIT; 
        break;
      }
    }
    break;
  case MAILCASE_END:
    //�f�[�^�擾
    if( param->mode == MAIL_MODE_CREATE){
      if( wk->dat->val == VIEW_END_DECIDE){
        MailSys_SetTmpData(param->pDat,wk->dat);  //���[���f�[�^����������
        //�X�R�A���Z����
        RECORD_Inc(SaveData_GetRecord(param->savedata), RECID_MAIL_WRITE );
        param->ret_val = 1; //�߂�l�ݒ�
      }else{
        param->ret_val = 0; //�߂�l�ݒ�
      }
    }
    return GFL_PROC_RES_FINISH;
  case WORDCASE_INIT:
    GFL_OVERLAY_Load( FS_OVERLAY_ID(pmsinput));
    OS_TPrintf("pms cntNo = %d\n", wk->dat->cntNo);
    // �ȈՉ�b���̓��[�h�ݒ�
    _set_pmsinput_mode(wk, param);
    // �ȈՉ�b�Ăяo��
    GFL_PROC_LOCAL_CallProc( wk->localProcSys, NO_OVERLAY_ID, &PMSProcData,  wk->app_wk );
    *seq     = WORDCASE_WAIT;
    break;
  case WORDCASE_WAIT:
    if(proc_status != GFL_PROC_MAIN_VALID){
      // �ȈՉ�b�擾
      if( PMSI_PARAM_CheckCanceled(wk->app_wk) == FALSE)
      {
        //�ύX������Ώ����߂�
        if(wk->dat->cntNo<3){
          PMSI_PARAM_GetInputDataSentence( wk->app_wk,  &(wk->dat->msg[wk->dat->cntNo]));
        }else{
          wk->dat->pmsword = PMSI_PARAM_GetInputDataSingle( wk->app_wk );
          OS_TPrintf("�ȈՒP�� %d���Z�b�g\n", wk->dat->pmsword);
        }
      }
      PMSI_PARAM_Delete(wk->app_wk);
      GFL_OVERLAY_Unload( FS_OVERLAY_ID(pmsinput));
      *seq = MAILCASE_INIT; 
    }
    break;
  }
  return GFL_PROC_RES_CONTINUE;
}

/**
 *  @brief  ���[���쐬�I��
 */
GFL_PROC_RESULT MailSysProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  MAIL_MAIN_DAT *wk = mywk;

  // ���[���V�X�e����PROCSYS�I��
  GFL_PROC_LOCAL_Exit( wk->localProcSys );

  //���[���f�[�^�e���|�����̈���
  MailSys_ReleaseTmpData(wk->dat);

  //���[�N�G���A���
  GFL_PROC_FreeWork(proc);
  
  GFL_HEAP_CheckHeapSafe(wk->heapID);

  GFL_HEAP_DeleteHeap(wk->heapID);

  return GFL_PROC_RES_FINISH;
}



//----------------------------------------------------------------------------------
/**
 * @brief �ȈՉ�b���͉�ʌĂяo�����̃��[�h�ݒ�
 *
 * @param   wk    
 * @param   param   
 */
//----------------------------------------------------------------------------------
static void _set_pmsinput_mode( MAIL_MAIN_DAT* wk, MAIL_PARAM* param )
{
  // �ȈՉ�b���̓��[�h�Q�i��^���E���R���͕��j
  // �ȈՉ�b�P�P����̓��[�h�P�i���͖����j�̕�����s��
  
  // �P����͂̂�
  if( wk->dat->cntNo == 3 ){
    // ���͌Œ胂�[�h�Ăяo��
    PMSDAT_Clear( &wk->tmpPms );
    wk->tmpPms.sentence_type = PMS_TYPE_SYSTEM;
    wk->tmpPms.sentence_id   = pmss_system_01;
    wk->tmpPms.word[0]       = wk->dat->pmsword;

    wk->app_wk = PMSI_PARAM_Create( PMSI_MODE_SINGLE, PMSI_GUIDANCE_DEFAULT,
                                    NULL,TRUE,param->savedata,wk->heapID);
    PMSI_PARAM_SetInitializeDataSingle( wk->app_wk, wk->dat->pmsword );
  // ���͓���
  }else{
    // ���͌Œ胂�[�h�Ăяo��
    if(wk->dat->pms_condition[wk->dat->cntNo]==1){
      PMSDAT_Copy(&(wk->tmpPms),&(wk->dat->msg[wk->dat->cntNo]));
      wk->app_wk = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT,
                                      &(wk->tmpPms),TRUE,param->savedata,wk->heapID);
    }else{
      // ���R���̓��[�h�Ăяo���i������ɂ������APARAM_Create��NULL�ōs�������
      // SetInitializeDataSentence������Ǝ��R���͂ɁACreate�̎���PMS��ݒ肵�Ă���ƌŒ蕶�͂ɂȂ�
      wk->app_wk = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT,
                                      NULL,TRUE,param->savedata,wk->heapID);
  
      //�����f�[�^�Z�b�g
      if(PMSDAT_IsEnabled(&(wk->dat->msg[wk->dat->cntNo]))){
        //���ɓ��͕�������
        PMSDAT_Copy(&(wk->tmpPms),&(wk->dat->msg[wk->dat->cntNo]));
      }else{
        //�܂���
        PMSDAT_Init(&(wk->tmpPms),PMS_TYPE_MAIL); 
      }
      PMSI_PARAM_SetInitializeDataSentence(wk->app_wk,&(wk->tmpPms));
    }
  }
}

/**
 *  @brief  ���[���f�[�^�@�e���|�����쐬
 *
 *  �������Ń��������A���P�[�g����̂ŕK���Ăяo������
 *  �@MailSys_ReleaseTmpData()�ŉ�����邱��
 */


//----------------------------------------------------------------------------------
/**
 * @brief ���[���f�[�^�@�e���|�����쐬
 *  �������Ń��������A���P�[�g����̂ŕK���Ăяo������
 *  �@MailSys_ReleaseTmpData()�ŉ�����邱��
 *
 * @param   org   
 * @param   heapID    
 *
 * @retval  MAIL_TMP_DATA*    
 */
//----------------------------------------------------------------------------------
static MAIL_TMP_DATA* MailSys_AllocTmpData(const MAIL_DATA* org,int heapID)
{
  u16 i;
  MAIL_TMP_DATA* tmp;

  tmp = GFL_HEAP_AllocMemory( heapID, sizeof(MAIL_TMP_DATA) );
  MI_CpuClearFast(tmp,sizeof(MAIL_TMP_DATA));
  
  tmp->val = 0;

  tmp->writerID = MailData_GetWriterID(org);
  tmp->name = GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, heapID );
  GFL_STR_SetStringCode( tmp->name, MailData_GetWriterName((MAIL_DATA*)org) );
  tmp->design  = MailData_GetDesignNo(org);
  tmp->lang    = MailData_GetCountryCode(org);
  tmp->ver     = MailData_GetCasetteVersion(org);
  tmp->pmsword = MailData_GetPmsWord(org);

  for(i = 0;i < MAILDAT_MSGMAX;i++){
    PMSDAT_Copy( &tmp->msg[i], MailData_GetMsgByIndex((MAIL_DATA*)org, i) );
  }
  return tmp;
}


//----------------------------------------------------------------------------------
/**
 * @brief ���[���f�[�^�@�e���|�������
 *  ��MailSys_AllocTmpData()�Ŋm�ۂ����������̈���J������
 *
 * @param   tmp   
 */
//----------------------------------------------------------------------------------
static void MailSys_ReleaseTmpData(MAIL_TMP_DATA* tmp)
{
  if(tmp->name != NULL){
    GFL_STR_DeleteBuffer(tmp->name);
  }
  GFL_HEAP_FreeMemory(tmp);
}


//----------------------------------------------------------------------------------
/**
 * @brief ���[���f�[�^�@�e���|�����f�[�^�����o��
 *
 * @param   org   ���͍ς݃��[���f�[�^
 * @param   tmp   �����o����
 */
//----------------------------------------------------------------------------------
static void MailSys_SetTmpData( MAIL_DATA* org, MAIL_TMP_DATA* tmp )
{
  u16 i;

  //�ȈՕ��R�s�[
  for(i = 0;i < MAILDAT_MSGMAX;i++){
    MailData_SetMsgByIndex(org,&tmp->msg[i],i);
  }

  // �ȈՒP��R�s�[
  MailData_SetPmsWord( org, tmp->pmsword );

  //�f�[�^��L�������邽�߂Ƀf�U�C��NO��ݒ�
  MailData_SetDesignNo(org,tmp->design);
}

