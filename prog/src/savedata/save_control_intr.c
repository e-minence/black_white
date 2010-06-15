//==============================================================================
/**
 * @file    save_control_intr.c
 * @brief   �C���g����ʗp�A���f�@�\�t���Z�[�u�V�X�e��
 * @author  matsuda
 * @date    2009.12.10(��)
 *
 * �C���g����ʂ̗��ŏ���Z�[�u�����s���Ă����A�J�[�h�A�N�Z�X�������ꍇ��
 * ���f�����������邱�ƂŁA�J�[�h�A�N�Z�X�ł���^�C�~���O�����V�X�e���ł�
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control_intr.h"
#include "savedata/player_data.h"


//==============================================================================
//  �萔��`
//==============================================================================
typedef enum{
  INTR_SAVE_STATUS_NULL,          ///<�����������Ă��Ȃ�
  INTR_SAVE_STATUS_MAIN,          ///<�Z�[�u���s��
  INTR_SAVE_STATUS_SUSPEND,       ///<�Z�[�u�͒��f���Ă��܂�
  INTR_SAVE_STATUS_FINISH,        ///<�Z�[�u����
}INTR_SAVE_STATUS;


//==============================================================================
//  �\���̒�`
//==============================================================================
struct _INTR_SAVE_CONTROL{
  SAVE_CONTROL_WORK *ctrl;    ///<�Z�[�u�V�X�e���ւ̃|�C���^
  
  u8 status;                  ///<�Z�[�u��(INTR_SAVE_STATUS)
  u8 backup_status;           ///<���f���A���O�̃Z�[�u�󋵂��o�b�N�A�b�v
  u8 suspend_req;             ///<TRUE:���f���N�G�X�g����
  u8 no_save;                 ///<TRUE:���ɃZ�[�u�f�[�^������̂ŃZ�[�u�͂��Ȃ�
  
  BOOL outside_exists;        ///<�Ǘ��O�Z�[�u���݃t���O�̑ޔ��o�b�t�@
  BOOL outside_break;         ///<�Ǘ��O�Z�[�u�j���t���O�̑ޔ��o�b�t�@
};


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _SoftReset_Callback_SaveCancel(void *work);


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �C���g����ʗp�A���f�@�\�t���Z�[�u�V�X�e���쐬
 *
 * @param   heap_id		
 * @param   ctrl		
 *
 * @retval  INTR_SAVE_CONTROL *		
 */
//==================================================================
INTR_SAVE_CONTROL * IntrSave_Init(HEAPID heap_id, SAVE_CONTROL_WORK *ctrl)
{
  INTR_SAVE_CONTROL *isc;
  
  isc = GFL_HEAP_AllocClearMemory(heap_id, sizeof(INTR_SAVE_CONTROL));
  isc->ctrl = ctrl;
  if(SaveData_GetExistFlag(ctrl) == TRUE){
    isc->no_save = TRUE;
  }
  else{
    //�Z�[�u���̃\�t�g���Z�b�g�R�[���o�b�N�ݒ�
    GFL_UI_SoftResetSetFunc(_SoftReset_Callback_SaveCancel, isc);
  }
  
  return isc;
}

//==================================================================
/**
 * �C���g����ʗp�A���f�@�\�t���Z�[�u�V�X�e���폜
 *
 * @param   isc		
 */
//==================================================================
void IntrSave_Exit(INTR_SAVE_CONTROL *isc)
{
  //�Z�[�u���̃\�t�g���Z�b�g�R�[���o�b�N����
  GFL_UI_SoftResetSetFunc(NULL, NULL);
  
  if(isc->no_save == FALSE){
    GF_ASSERT(isc->status == INTR_SAVE_STATUS_FINISH);
  }
  GFL_HEAP_FreeMemory(isc);
}

//==================================================================
/**
 * �Z�[�u�J�n
 *
 * @param   isc		
 */
//==================================================================
void IntrSave_Start(INTR_SAVE_CONTROL *isc)
{
  if(isc->no_save == TRUE){
    return;
  }
  SaveControl_SaveAsyncInit(isc->ctrl);
  if(isc->no_save == FALSE){
    //����Z�b�g�A�b�v��Ԃɂ���
    SaveData_SetNowSaveModeSetupON(isc->ctrl);
    //�Ǘ��O�Z�[�u�ɑ΂��ĉ����A�N�V�������Ȃ��悤�ɑ��݃t���O��ޔ����A���������ɂ��Ă���
    isc->outside_exists = SaveData_GetOutsideExistFlag(isc->ctrl);
    isc->outside_break = SaveData_GetOutsideBreakFlag(isc->ctrl);
    SaveData_SetOutsideExistFlag(isc->ctrl, FALSE);
    SaveData_SetOutsideBreakFlag(isc->ctrl, FALSE);
  }
  isc->status = INTR_SAVE_STATUS_MAIN;
}

//==================================================================
/**
 * �Z�[�u���C��
 *
 * @param   isc		
 *
 * @retval  SAVE_RESULT		
 */
//==================================================================
SAVE_RESULT IntrSave_Main(INTR_SAVE_CONTROL *isc)
{
  SAVE_RESULT result;
  
  if(isc->no_save == TRUE){
    return SAVE_RESULT_OK;
  }
  
  switch(isc->status){
  case INTR_SAVE_STATUS_MAIN:
    if(isc->suspend_req == TRUE && GFL_BACKUP_SAVEASYNC_CheckCardLock() == FALSE){
      isc->backup_status = isc->status;
      isc->status = INTR_SAVE_STATUS_SUSPEND;
      OS_TPrintf("IntSave ... �J�[�hUnlock�m�F state�T�X�y���h\n");
      return SAVE_RESULT_CONTINUE;
    }
    
    result = SaveControl_SaveAsyncMain(isc->ctrl);
    if(result == SAVE_RESULT_OK || result == SAVE_RESULT_NG){
      isc->status = INTR_SAVE_STATUS_FINISH;
      OS_TPrintf("IntSave ... �Z�[�u�I��\n");
      //�Z�[�u���I�������̂ŊǗ��O�Z�[�u�̑��݃t���O�𕜋A
      SaveData_SetOutsideExistFlag(isc->ctrl, isc->outside_exists);
      SaveData_SetOutsideBreakFlag(isc->ctrl, isc->outside_break);
    }
    return result;
  
  case INTR_SAVE_STATUS_SUSPEND:
    if(isc->suspend_req == FALSE){
      isc->status = isc->backup_status;
      OS_TPrintf("�Z�[�u�͒��f���Ă��܂�...\n");
    }
    return SAVE_RESULT_CONTINUE;

  case INTR_SAVE_STATUS_FINISH:
    return SAVE_RESULT_OK;
  }
  
  return SAVE_RESULT_CONTINUE;
}

//==================================================================
/**
 * �Z�[�u���f���N�G�X�g
 *
 * @param   isc		
 *
 * ���ۂɒ��f���ꂽ����IntrSave_CheckSuspend�֐��ŊĎ����Ă�������
 */
//==================================================================
void IntrSave_ReqSuspend(INTR_SAVE_CONTROL *isc)
{
  if(isc->status == INTR_SAVE_STATUS_FINISH){
    return;
  }
  
  GF_ASSERT(isc->suspend_req == FALSE);
  isc->suspend_req = TRUE;
}

//==================================================================
/**
 * ���f���Ă����Z�[�u���ĊJ���܂�
 *
 * @param   isc		
 */
//==================================================================
void IntrSave_Resume(INTR_SAVE_CONTROL *isc)
{
  if(isc->status == INTR_SAVE_STATUS_FINISH){
    return;
  }
  
  GF_ASSERT(isc->suspend_req == TRUE);
  isc->suspend_req = FALSE;
}

//==================================================================
/**
 * �Z�[�u�𒆒f���Ă��邩���ׂ�
 *
 * @param   isc		
 *
 * @retval  BOOL		TRUE:���f���Ă���(�J�[�h�A�N�Z�X�\)
 * @retval  BOOL		FALSE:���f���Ă��Ȃ�(�J�[�h�A�N�Z�X�֎~)
 */
//==================================================================
BOOL IntrSave_CheckSuspend(INTR_SAVE_CONTROL *isc)
{
  if(isc->no_save == TRUE 
      || isc->status == INTR_SAVE_STATUS_FINISH){
    //�O���V�[�P���X�̐i�s��~�̌����ɂȂ�̂ŃZ�[�u���I���̎���TRUE��Ԃ�
    return TRUE;
  }
  
  if(isc->status == INTR_SAVE_STATUS_SUSPEND){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * ��1�i�K�̃Z�[�u��������AMYSTATUS�ɐ��ʁA���O���Z�b�g������A
 * ���̊֐����Ăяo���Ă�������
 * MYSTATUS�Z�b�g�������2�x�ڂ̃Z�[�u���s���܂�
 *
 * @param   isc		
 */
//==================================================================
void IntrSave_ReqMyStatusSave(INTR_SAVE_CONTROL *isc)
{
//  isc->mystatus_save_req = TRUE;
}

//==================================================================
/**
 * �S�ẴZ�[�u���������Ă��邩���ׂ�
 *
 * @param   isc		
 *
 * @retval  BOOL		TRUE:�S�ẴZ�[�u���������Ă���
 * @retval  BOOL		FALSE:�܂��Z�[�u���c���Ă���
 */
//==================================================================
BOOL IntrSave_CheckAllSaveEnd(INTR_SAVE_CONTROL *isc)
{
  if(isc->no_save == TRUE){
    return TRUE;
  }
  if(isc->status == INTR_SAVE_STATUS_FINISH){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �\�t�g�E�F�A���Z�b�g���O���ɌĂ΂��R�[���o�b�N�֐�
 *
 * @param   work		
 */
//--------------------------------------------------------------
static void _SoftReset_Callback_SaveCancel(void *work)
{
  INTR_SAVE_CONTROL *isc = work;
  
  OS_TPrintf("intr_save softreset callback\n");
  if(isc->no_save == FALSE && isc->status != INTR_SAVE_STATUS_FINISH){
    SaveControl_SaveAsyncCancel(isc->ctrl);
    isc->no_save = TRUE;  //�ꉞ�A�Z�[�u�����̕��ɂ����Ȃ��悤��no_save�����ɂ��Ă���
  }
}
