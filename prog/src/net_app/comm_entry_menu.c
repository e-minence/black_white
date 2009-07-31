//==============================================================================
/**
 * @file    comm_entry_menu.c
 * @brief   �Q����W���j���[����
 * @author  matsuda
 * @date    2009.07.28(��)
 */
//==============================================================================
#include <gflib.h>
#include "field/fieldmap.h"
#include "message.naix"
#include "msg/msg_connect.h"
#include "net_app/comm_entry_menu.h"
#include "print/wordset.h"


//==============================================================================
//  �萔��`
//==============================================================================
///ID��STRBUF�T�C�Y
#define ID_STRBUF_SIZE          (5)   //5���\���̈�
///EXPAND�œW�J���郁�b�Z�[�W�o�b�t�@�̃T�C�Y
#define EXPAND_STRBUF_SIZE      (100)

///���[�U�[�X�e�[�^�X
enum{
  USER_STATUS_NULL,         ///<���[�U�[�����Ȃ�
  USER_STATUS_READY,        ///<������(���X�g�ɂ܂����O���ڂ��Ă��Ȃ�)
  USER_STATUS_EXAMINATION,  ///<�R���Ώ�
  USER_STATUS_EXAMINATION_WAIT, ///<�R�������҂�
  USER_STATUS_COMPLETION,   ///<�G���g���[����
  USER_STATUS_REFUSE,       ///<�f����
  USER_STATUS_FAILE,        ///<���Ȃ��Ȃ���
};

///�G���g���[���X�gBMP�T�C�Y(�L�����P��)
enum{
  LISTBMP_POS_LEFT = 2,
  LISTBMP_POS_RIGHT = 20,
  LISTBMP_POS_TOP = 6,
  LISTBMP_POS_BOTTOM = LISTBMP_POS_TOP + 8,
};

///�G���g���[���X�g��BMP�������݈ʒu(�h�b�g�P��)
enum{
  LISTBMP_START_NAME_X = 0,
  LISTBMP_START_NAME_Y = 0,
  LISTBMP_START_ID_X = (LISTBMP_POS_RIGHT - 8) * 8,
  LISTBMP_START_ID_Y = LISTBMP_START_NAME_Y,
};

///�ŏI����
typedef enum{
  DECIDE_TYPE_NULL,
  DECIDE_TYPE_OK,
  DECIDE_TYPE_NG,
}DECIDE_TYPE;

//==============================================================================
//  �\���̒�`
//==============================================================================
///�G���g���[�҂̃f�[�^
typedef struct{
  STRBUF *strbuf_name;   ///<�G���g���[�҂̖��O
  STRBUF *strbuf_id;     ///<�G���g���[�҂�ID(�g���[�i�[�J�[�h��ID)
  u8 sex;
  u8 status;
  u8 force_entry;        ///<TRUE:�����I�Ɏ󂯓����
  u8 send_flag;          ///<TRUE:�Ԏ����M�ς�(�O�����痧�Ă�)
}COMM_ENTRY_USER;

///�u�͂��E�������v����
typedef struct{
  FLDMENUFUNC *menufunc;
  u8 seq;
}COMM_ENTRY_YESNO;

///�Q����W�Ǘ��V�X�e��
typedef struct _COMM_ENTRY_MENU_SYSTEM{
  FIELD_MAIN_WORK *fieldWork;
  WORDSET *wordset;
  GFL_MSGDATA *msgdata;
  FLDMSGWIN *fld_msgwin;
  FLDMSGWIN_STREAM *fld_stream;
  STRBUF *strbuf_expand;
  
  COMM_ENTRY_USER user[COMM_ENTRY_USER_MAX];    ///<�G���g���[�҂̃f�[�^
  COMM_ENTRY_USER *examination_user;             ///<�R�����̃��[�U�[
  COMM_ENTRY_YESNO yesno;                       ///<�u�͂��E�������v����
  
  u8 min_num;         ///<�ŏ��l��
  u8 max_num;         ///<�ő�l��
  u8 seq;
  u8 padding;
}COMM_ENTRY_MENU_SYSTEM;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void CommEntryMenu_ListUpdate(COMM_ENTRY_MENU_PTR em);
static BOOL CommEntryMenu_SetExamination(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user);
static void CommEntryMenu_ExaminationUpdate(COMM_ENTRY_MENU_PTR em);
static DECIDE_TYPE CommEntryMenu_DecideUpdate(COMM_ENTRY_MENU_PTR em);
static BOOL CommEntryMenu_InterruptCheck(COMM_ENTRY_MENU_PTR em);
static void CommEntryMenu_NameDraw(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user, int netID);
static void CommEntryMenu_NameErase(COMM_ENTRY_MENU_PTR em, int netID);
static void _StreamMsgSet(COMM_ENTRY_MENU_PTR em, u32 msg_id);



//==================================================================
/**
 * �Q����W���j���[�F�Z�b�g�A�b�v
 *
 * @param   fieldWork		
 * @param   min_num		  �ŏ��l��
 * @param   max_num		  �ő�l��
 * @param   heap_id		  �q�[�vID
 *
 * @retval  COMM_ENTRY_MENU_PTR		���������Q����W���j���[���[�N�ւ̃|�C���^
 */
//==================================================================
COMM_ENTRY_MENU_PTR CommEntryMenu_Setup(FIELD_MAIN_WORK *fieldWork, int min_num, int max_num, HEAPID heap_id)
{
  COMM_ENTRY_MENU_PTR em;
  FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
  int i;
  
  GF_ASSERT(max_num <= COMM_ENTRY_USER_MAX);
  
  em = GFL_HEAP_AllocClearMemory(heap_id, sizeof(COMM_ENTRY_MENU_SYSTEM));
  
  em->wordset = WORDSET_Create( heap_id );
  em->msgdata = FLDMSGBG_CreateMSGDATA( fldmsg_bg, NARC_message_connect_dat );
  em->fld_msgwin = FLDMSGWIN_Add( fldmsg_bg, em->msgdata, 
    LISTBMP_POS_LEFT, LISTBMP_POS_TOP, 
    LISTBMP_POS_RIGHT - LISTBMP_POS_LEFT, LISTBMP_POS_BOTTOM - LISTBMP_POS_TOP);
  em->fld_stream = FLDMSGWIN_STREAM_AddTalkWin(fldmsg_bg, em->msgdata);
  em->strbuf_expand = GFL_STR_CreateBuffer(EXPAND_STRBUF_SIZE, heap_id);
  em->min_num = min_num;
  em->max_num = max_num;
  em->fieldWork = fieldWork;
  
  for(i = 0; i < COMM_ENTRY_USER_MAX; i++){
    em->user[i].strbuf_name = GFL_STR_CreateBuffer(PERSON_NAME_SIZE + EOM_SIZE, heap_id);
    em->user[i].strbuf_id = GFL_STR_CreateBuffer(ID_STRBUF_SIZE + EOM_SIZE, heap_id);
  }

  //�ŏ��̃��b�Z�[�W�`��
  _StreamMsgSet(em, msg_connect_02_01);
  
  return em;
}

//==================================================================
/**
 * �Q����W���j���[�폜
 *
 * @param   em		
 */
//==================================================================
void CommEntryMenu_Exit(COMM_ENTRY_MENU_PTR em)
{
  int i;
  
  for(i = 0; i < COMM_ENTRY_USER_MAX; i++){
    GFL_STR_DeleteBuffer(em->user[i].strbuf_name);
    GFL_STR_DeleteBuffer(em->user[i].strbuf_id);
  }
  GFL_STR_DeleteBuffer(em->strbuf_expand);
  
  FLDMSGWIN_STREAM_Delete(em->fld_stream);
  FLDMSGWIN_Delete(em->fld_msgwin);
  FLDMSGBG_DeleteMSGDATA(em->msgdata);
  WORDSET_Delete( em->wordset );
  
  GFL_HEAP_FreeMemory(em);
}

//==================================================================
/**
 * �v���C���[�G���g���[
 *
 * @param   em		  
 * @param   netID		      �l�b�gID
 * @param   name		      ���O
 * @param   id		        ID(�g���[�i�[�J�[�h��ID)
 * @param   sex           ����
 * @param   force_entry		TRUE:�����󂯓���B�@FALSE:�󂯓���邩�I������
 */
//==================================================================
void CommEntryMenu_Entry(COMM_ENTRY_MENU_PTR em, int netID, const STRCODE *name, u32 id, u8 sex, BOOL force_entry)
{
  COMM_ENTRY_USER *p_user = &em->user[netID];
  
  GF_ASSERT(netID <= COMM_ENTRY_USER_MAX);
  
  GFL_STR_SetStringCode(p_user->strbuf_name, name);
  STRTOOL_SetNumber(p_user->strbuf_id, id & 0xffff, ID_STRBUF_SIZE, 
    STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU);
  p_user->sex = sex;
  p_user->status = USER_STATUS_READY;
  p_user->force_entry = force_entry;
}

//==================================================================
/**
 * �Q����W���j���[�F�X�V����
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE:�S�Ă̏������I���B
 */
//==================================================================
BOOL CommEntryMenu_Update(COMM_ENTRY_MENU_PTR em)
{
  switch(em->seq){
  case 0:   //��W��
    //���X�g�X�V
    CommEntryMenu_ListUpdate(em);
    //�G���g���[�҂��󂯓���邩�I��
    CommEntryMenu_ExaminationUpdate(em);
    if(CommEntryMenu_InterruptCheck(em) == TRUE){
      int entry_num = CommEntryMenu_GetCompletionNum(em);
      if(entry_num >= em->max_num
          || (entry_num >= em->min_num && (GFL_UI_KEY_GetTrg() & PAD_BUTTON_A))){
        em->seq++;
      }
    }
    break;
  case 1:   //���̃����o�[�ł�낵���ł����H
    {
      DECIDE_TYPE type;
      type = CommEntryMenu_DecideUpdate(em);
      if(type == DECIDE_TYPE_OK){
        em->seq++;
      }
      else if(type == DECIDE_TYPE_NG){
        em->seq--;
      }
    }
    break;
  case 2:   //�I������
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���X�g�X�V����
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void CommEntryMenu_ListUpdate(COMM_ENTRY_MENU_PTR em)
{
  int i;
  COMM_ENTRY_USER *user = em->user;

  //���X�g�̖��O�`��
  for(i = 0; i < COMM_ENTRY_USER_MAX; i++){
    switch(user->status){
    case USER_STATUS_READY:
      CommEntryMenu_NameDraw(em, user, i);
      if(user->force_entry == TRUE){
        user->status = USER_STATUS_COMPLETION;
      }
      else{
        user->status = USER_STATUS_EXAMINATION;
      }
      break;
    case USER_STATUS_EXAMINATION:
      if(CommEntryMenu_SetExamination(em, user) == TRUE){
        user->status = USER_STATUS_EXAMINATION_WAIT;
      }
      break;
    case USER_STATUS_FAILE:
      CommEntryMenu_NameErase(em, i);
      GFL_STD_MemClear(user, sizeof(COMM_ENTRY_USER));
      break;
    }
    
    user++;
  }
}

//--------------------------------------------------------------
/**
 * �R���ΏۂɃ��[�U�[��o�^����
 *
 * @param   em		
 * @param   user		
 *
 * @retval  BOOL		TRUE:�o�^����
 */
//--------------------------------------------------------------
static BOOL CommEntryMenu_SetExamination(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user)
{
  if(em->examination_user == NULL){
    em->examination_user = user;
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �G���g���[�҂��󂯓���邩�I��
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void CommEntryMenu_ExaminationUpdate(COMM_ENTRY_MENU_PTR em)
{
  COMM_ENTRY_USER *user = em->examination_user;
  COMM_ENTRY_YESNO *yesno = &em->yesno;
  FLDMSGBG *fldmsg_bg;
  
  if(user == NULL || FLDMSGWIN_STREAM_Print(em->fld_stream) == FALSE){
    return;
  }
  
  switch(yesno->seq){
  case 0:
    WORDSET_RegisterWord(em->wordset, 0, user->strbuf_name, user->sex, TRUE, PM_LANG);
    _StreamMsgSet(em, msg_connect_02_02);
    yesno->seq++;
    break;
  case 1:
    GF_ASSERT(yesno->menufunc == NULL);
    fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
    yesno->menufunc = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
    yesno->seq++;
    break;
  case 2:
    {
      FLDMENUFUNC_YESNO ret;
      
      ret = FLDMENUFUNC_ProcYesNoMenu(yesno->menufunc);
      if(ret == FLDMENUFUNC_YESNO_NULL){
        break;
      }
      else if(ret == FLDMENUFUNC_YESNO_YES){
        user->status = USER_STATUS_COMPLETION;
      }
      else{
      #if 0
        user->status = USER_STATUS_REFUSE;
      #else
        user->status = USER_STATUS_COMPLETION;
      #endif
      }
      FLDMENUFUNC_DeleteMenu(yesno->menufunc);
      yesno->menufunc = NULL;
      _StreamMsgSet(em, msg_connect_02_01);
      em->examination_user = NULL;
      yesno->seq = 0;
    }
    break;
  }
}

//--------------------------------------------------------------
/**
 * �ŏI���菈��
 *
 * @param   em		
 *
 * @retval  
 */
//--------------------------------------------------------------
static DECIDE_TYPE CommEntryMenu_DecideUpdate(COMM_ENTRY_MENU_PTR em)
{
  FLDMSGBG *fldmsg_bg;
  COMM_ENTRY_YESNO *yesno = &em->yesno;

  if(FLDMSGWIN_STREAM_Print(em->fld_stream) == FALSE){
    return DECIDE_TYPE_NULL;
  }

  switch(yesno->seq){
  case 0:
    _StreamMsgSet(em, msg_connect_02_03);
    yesno->seq++;
    break;
  case 1:
    GF_ASSERT(yesno->menufunc == NULL);
    fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
    yesno->menufunc = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
    yesno->seq++;
    break;
  case 2:
    {
      FLDMENUFUNC_YESNO ret;
      
      ret = FLDMENUFUNC_ProcYesNoMenu(yesno->menufunc);
      if(ret == FLDMENUFUNC_YESNO_NULL){
        break;
      }
      
      FLDMENUFUNC_DeleteMenu(yesno->menufunc);
      yesno->menufunc = NULL;
      yesno->seq = 0;
      if(ret == FLDMENUFUNC_YESNO_YES){
        return DECIDE_TYPE_OK;
      }
      else{
      #if 0
        return DECIDE_TYPE_NG;
      #else
        return DECIDE_TYPE_OK;
      #endif
      }
    }
    break;
  }
  return DECIDE_TYPE_NULL;
}

//==================================================================
/**
 * �G���g���[���ʂ̎擾
 *
 * @param   em		
 * @param   netID		
 * @param   exit_mode   TRUE:�e���Q�������o�[�������A�܂��Ԏ��𑗐M���Ă��Ȃ������o�[���擾���郂�[�h�ł�(FALSE�̎��̈Ⴂ�́ATRUE�̏ꍇ�͐R���ɂ���R�ꂽ�҂̏�Ԃ��Ԃ����ł�)
 *
 * @retval  COMM_ENTRY_ANSWER		
 *
 * ���M�ς݃t���O�������Ă�����̂�NULL��Ԃ��܂�
 */
//==================================================================
COMM_ENTRY_ANSWER CommEntryMenu_GetAnswer(COMM_ENTRY_MENU_PTR em, int netID, BOOL exit_mode)
{
  if(em->user[netID].send_flag == FALSE){
    if(em->user[netID].status == USER_STATUS_COMPLETION){
      return COMM_ENTRY_ANSWER_COMPLETION;
    }
    else if(em->user[netID].status == USER_STATUS_REFUSE){
//      return COMM_ENTRY_ANSWER_REFUSE;
      return COMM_ENTRY_ANSWER_COMPLETION;
    }
    
    if(exit_mode == TRUE){
      if(em->user[netID].status != USER_STATUS_NULL && em->user[netID].status != USER_STATUS_FAILE){
        return COMM_ENTRY_ANSWER_REFUSE;
      }
    }
  }
  return COMM_ENTRY_ANSWER_NULL;
}

//==================================================================
/**
 * ���M�ς݃t���O�𗧂Ă�
 *
 * @param   em		
 * @param   netID		
 *
 * CommEntryMenu_GetAnswer�Ŏ擾�����Ԏ���ʐM����ɑ��M������A����Ńt���O�𗧂ĂĂ�������
 * CommEntryMenu_GetAnswer��NULL���Ԃ�悤�ɂȂ�̂�2�d���M�Ȃǂ��N���Ȃ��Ȃ�܂�
 * ���ʂ��u���ہv�������ꍇ�̓��X�g����폜���܂�
 */
//==================================================================
void CommEntryMenu_SetSendFlag(COMM_ENTRY_MENU_PTR em, int netID)
{
  GF_ASSERT(em->user[netID].status == USER_STATUS_COMPLETION || em->user[netID].status == USER_STATUS_REFUSE);
  
  if(em->user[netID].status == USER_STATUS_COMPLETION){
    em->user[netID].send_flag = TRUE;
  }
  else if(em->user[netID].status == USER_STATUS_REFUSE){
    CommEntryMenu_NameErase(em, netID);
    GFL_STD_MemClear(&em->user[netID], sizeof(COMM_ENTRY_USER));
  }
}

//==================================================================
/**
 * �G���g���[���������l�����擾����
 *
 * @param   em		
 *
 * @retval  int		�G���g���[�����l��
 */
//==================================================================
int CommEntryMenu_GetCompletionNum(COMM_ENTRY_MENU_PTR em)
{
  int i, count = 0;
  
  for(i = 0; i < COMM_ENTRY_USER_MAX; i++){
    if(em->user[i].status == USER_STATUS_COMPLETION){
      count++;
    }
  }
  return count;
}

//--------------------------------------------------------------
/**
 * ���̑I�����s���Ă��Ȃ��t���[�̏󋵂����ׂ�
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE�F�t���[
 */
//--------------------------------------------------------------
static BOOL CommEntryMenu_InterruptCheck(COMM_ENTRY_MENU_PTR em)
{
  if(em->examination_user == NULL && FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���O�`��
 *
 * @param   em		
 * @param   user		
 * @param   netID		
 */
//--------------------------------------------------------------
static void CommEntryMenu_NameDraw(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user, int netID)
{
  FLDMSGWIN_PrintStrBuf(em->fld_msgwin, 
    LISTBMP_START_NAME_X, LISTBMP_START_NAME_Y + netID*8*2, user->strbuf_name);
  FLDMSGWIN_PrintStrBuf(em->fld_msgwin, 
    LISTBMP_START_ID_X, LISTBMP_START_ID_Y + netID*8*2, user->strbuf_id);
}

//--------------------------------------------------------------
/**
 * ���O����
 *
 * @param   em		
 * @param   netID		
 */
//--------------------------------------------------------------
static void CommEntryMenu_NameErase(COMM_ENTRY_MENU_PTR em, int netID)
{
  FLDMSGWIN_FillClearWindow(em->fld_msgwin, 
    LISTBMP_START_NAME_X, LISTBMP_START_NAME_Y + netID*8*2, 
    (LISTBMP_POS_RIGHT - LISTBMP_POS_LEFT) * 8, LISTBMP_START_NAME_Y + netID*8*4);
  OS_TPrintf("Name Erase = %d, %d, %d, %d\n",     LISTBMP_START_NAME_X, LISTBMP_START_NAME_Y + netID*8*2, 
    (LISTBMP_POS_RIGHT - LISTBMP_POS_LEFT) * 8, LISTBMP_START_NAME_Y + netID*8*4);

}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�X�g���[���`��
 *
 * @param   em		
 * @param   msg_id		���b�Z�[�WID
 *
 * WORDSET�͂��炩���߃Z�b�g������ŌĂ�ł�������
 */
//--------------------------------------------------------------
static void _StreamMsgSet(COMM_ENTRY_MENU_PTR em, u32 msg_id)
{
  STRBUF *temp_strbuf;
  
  temp_strbuf = GFL_MSG_CreateString( em->msgdata, msg_id );
  WORDSET_ExpandStr( em->wordset, em->strbuf_expand, temp_strbuf );
  GFL_STR_DeleteBuffer(temp_strbuf);
  
  FLDMSGWIN_STREAM_ClearMessage(em->fld_stream);
  FLDMSGWIN_STREAM_PrintStrBufStart(em->fld_stream, 0, 0, em->strbuf_expand);
}
