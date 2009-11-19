//==============================================================================
/**
 * @file    intrude_message.c
 * @brief   �N���C�x���g�F���b�Z�[�W���ʏ���
 * @author  matsuda
 * @date    2009.10.27(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "field/fieldmap.h"
#include "field/scrcmd.h"
#include "intrude_message.h"
#include "message.naix"
#include "msg/msg_invasion.h"
#include "field/field_comm/intrude_types.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�N���C�x���g�Ŏg�p����q�[�vID
#define INTRUDE_EVENT_HEAPID        (HEAPID_PROC)
///�N���C�x���g�Ŏg�p���郁�b�Z�[�W�o�b�t�@�T�C�Y
#define INTRUDE_EVENT_MSGBUF_SIZE   (256)


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void IntrudeEventPrint_ClearStream(INTRUDE_EVENT_MSGWORK *iem);
static void IntrudeEventPrint_SetupMenu(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys,
  const FLDMENUFUNC_LIST *menulist, int list_max, const FLDMENUFUNC_HEADER *head);


//==============================================================================
//
//  �N���C�x���g���ʃ��b�Z�[�W����
//
//==============================================================================
//--------------------------------------------------------------
/**
 * �v�����g����Z�b�g�A�b�v
 *
 * @param   iem		
 * @param   gsys		
 */
//--------------------------------------------------------------
void IntrudeEventPrint_SetupFieldMsg(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys)
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
  
  iem->wordset = WORDSET_Create(INTRUDE_EVENT_HEAPID);
  iem->msgbuf = GFL_STR_CreateBuffer( INTRUDE_EVENT_MSGBUF_SIZE, INTRUDE_EVENT_HEAPID );
  iem->tmpbuf = GFL_STR_CreateBuffer( INTRUDE_EVENT_MSGBUF_SIZE, INTRUDE_EVENT_HEAPID );
  iem->msgdata = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
  iem->msgwin_stream = FLDMSGWIN_STREAM_AddTalkWin( msgBG, iem->msgdata );
  iem->print_flag = FALSE;
}

//--------------------------------------------------------------
/**
 * �v�����g����폜
 *
 * @param   iem		
 */
//--------------------------------------------------------------
void IntrudeEventPrint_ExitFieldMsg(INTRUDE_EVENT_MSGWORK *iem)
{
  if(iem->msgwin_stream != NULL){
    FLDMSGWIN_STREAM_Delete(iem->msgwin_stream);
    FLDMSGBG_DeleteMSGDATA(iem->msgdata);
    GFL_STR_DeleteBuffer(iem->tmpbuf);
    GFL_STR_DeleteBuffer(iem->msgbuf);
    WORDSET_Delete(iem->wordset);
    
    iem->msgwin_stream = NULL;  //����������̈�
  }
}

//--------------------------------------------------------------
/**
 * �v�����g�X�g���[���o��
 *
 * @param   iem		
 * @param   msg_id		
 */
//--------------------------------------------------------------
void IntrudeEventPrint_StartStream(INTRUDE_EVENT_MSGWORK *iem, u16 msg_id)
{
  if(iem->print_flag == TRUE){
    IntrudeEventPrint_ClearStream(iem);
  }
  
  GFL_MSG_GetString( iem->msgdata, msg_id, iem->tmpbuf );
  WORDSET_ExpandStr( iem->wordset, iem->msgbuf, iem->tmpbuf );

  FLDMSGWIN_STREAM_PrintStrBufStart( iem->msgwin_stream, 0, 0, iem->msgbuf );
  iem->print_flag = TRUE;
}

//==================================================================
/**
 * ���b�Z�[�W�̈���N���A
 *
 * @param   iem		
 */
//==================================================================
static void IntrudeEventPrint_ClearStream(INTRUDE_EVENT_MSGWORK *iem)
{
  GF_ASSERT(iem->msgwin_stream != NULL);
  FLDMSGWIN_STREAM_ClearMessage(iem->msgwin_stream);
  iem->print_flag = FALSE;
}

//--------------------------------------------------------------
/**
 * �v�����g�X�g���[���o�͑҂�
 *
 * @param   iem		
 *
 * @retval  BOOL TRUE=�\���I��,FALSE=�\����
 */
//--------------------------------------------------------------
BOOL IntrudeEventPrint_WaitStream(INTRUDE_EVENT_MSGWORK *iem)
{
  return FLDMSGWIN_STREAM_Print(iem->msgwin_stream);
}

//==================================================================
/**
 * ���X�g�L�[�҂�
 *
 * @retval  BOOL		TRUE�F���X�g�L�[�������ꂽ
 */
//==================================================================
BOOL IntrudeEventPrint_LastKeyWait(void)
{
  if( GFL_UI_KEY_GetTrg() & EVENT_WAIT_LAST_KEY ){
    return TRUE;
  }
  return FALSE;
}




//==============================================================================
//
//  ���j���[
//
//==============================================================================
///��b���C�����j���[���X�g
static const FLDMENUFUNC_LIST TalkMainMenuList[] =
{
  {msg_talk_menu_001, (void*)INTRUDE_TALK_STATUS_BATTLE},   //�ΐ�
  {msg_talk_menu_002, (void*)INTRUDE_TALK_STATUS_ITEM},     //�A�C�e���n��
  {msg_talk_menu_003, (void*)INTRUDE_TALK_STATUS_CANCEL},   //�L�����Z��
};

///���j���[�w�b�_�[(���C�����j���[�p)
static const FLDMENUFUNC_HEADER MenuHeader_MainMenu =
{
	3,		//���X�g���ڐ�
	3,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	12,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	0,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_NON,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	16,		//�����T�C�YY(�h�b�g
	32-15,		//�\�����WX �L�����P��
	1,		//�\�����WY �L�����P��
	14,		//�\���T�C�YX �L�����P��
	6,		//�\���T�C�YY �L�����P��
};

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 * @param   menulist		
 * @param   list_max		
 */
//--------------------------------------------------------------
static void IntrudeEventPrint_SetupMenu(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys,
  const FLDMENUFUNC_LIST *menulist, int list_max, const FLDMENUFUNC_HEADER *head)
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
  FLDMENUFUNC_LISTDATA *fldmenu_listdata;
  
  GF_ASSERT(iem->fldmenu_func == NULL && iem->msgdata != NULL);
  
  fldmenu_listdata = FLDMENUFUNC_CreateMakeListData(
      menulist, list_max, iem->msgdata, INTRUDE_EVENT_HEAPID);
  iem->fldmenu_func = FLDMENUFUNC_AddMenu(msgBG, head, fldmenu_listdata);
}

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�폜
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
void IntrudeEventPrint_ExitMenu(INTRUDE_EVENT_MSGWORK *iem)
{
  if(iem->fldmenu_func != NULL){
    FLDMENUFUNC_DeleteMenu(iem->fldmenu_func);
    iem->fldmenu_func = NULL;
  }
}

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�I��҂�
 *
 * @param   unisys		
 *
 * @retval  u32		����
 */
//--------------------------------------------------------------
u32 IntrudeEventPrint_SelectMenu(INTRUDE_EVENT_MSGWORK *iem)
{
  GF_ASSERT(iem->fldmenu_func != NULL);
  return FLDMENUFUNC_ProcMenu(iem->fldmenu_func);
}

//--------------------------------------------------------------
/**
 * ���C�����j���[���X�g�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 * @param   menulist		
 * @param   list_max		
 */
//--------------------------------------------------------------
void IntrudeEventPrint_SetupMainMenu(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys)
{
  IntrudeEventPrint_SetupMenu(iem, gsys, 
    TalkMainMenuList, NELEMS(TalkMainMenuList), &MenuHeader_MainMenu);
}

//==============================================================================
//  �͂�/������ �I�����j���[
//==============================================================================
//==================================================================
/**
 * �͂�/������ �I�����j���[�F�Z�b�g�A�b�v
 *
 * @param   iem		
 * @param   gsys		
 */
//==================================================================
void IntrudeEventPrint_SetupYesNo(INTRUDE_EVENT_MSGWORK *iem, GAMESYS_WORK *gsys)
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(fieldWork);

  GF_ASSERT(iem->fldmenu_func == NULL && iem->msgdata != NULL);
  iem->fldmenu_func = FLDMENUFUNC_AddYesNoMenu(msgBG, FLDMENUFUNC_YESNO_YES);
}

//--------------------------------------------------------------
/**
 * �͂�/������ �I�����j���[�F�폜
 *
 * @param   iem		
 */
//--------------------------------------------------------------
void IntrudeEventPrint_ExitYesNo(INTRUDE_EVENT_MSGWORK *iem)
{
  IntrudeEventPrint_ExitMenu(iem);
}

//--------------------------------------------------------------
/**
 * �͂�/������ �I�����j���[�F�I��҂�
 *
 * @param   iem		
 *
 * @retval  FLDMENUFUNC_YESNO		����
 */
//--------------------------------------------------------------
FLDMENUFUNC_YESNO IntrudeEventPrint_SelectYesNo(INTRUDE_EVENT_MSGWORK *iem)
{
  GF_ASSERT(iem->fldmenu_func != NULL);
  return FLDMENUFUNC_ProcYesNoMenu(iem->fldmenu_func);
}
