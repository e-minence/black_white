//==============================================================================
/**
 * @file    union_msg.c
 * @brief   ���j�I�����[���F�c�[����(�I�[�o�[���C�̈�ɔz�u����܂�)
 * @author  matsuda
 * @date    2009.07.13(��)
 */
//==============================================================================
#include <gflib.h>
#include "union_types.h"
#include "union_local.h"
#include "net_app/union/union_main.h"
#include "field/field_player.h"
#include "field/fieldmap.h"
#include "net_app/union/union_msg.h"
#include "message.naix"
#include "msg/msg_union.h"
#include "system/main.h"


//==============================================================================
//  �f�[�^
//==============================================================================
//--------------------------------------------------------------
//  ���j���[�w�b�_�[
//--------------------------------------------------------------
///���C�����j���[���X�g
static const FLDMENUFUNC_LIST MainMenuList[] =
{
  {msg_union_select_01, (void*)UNION_MSG_MENU_SELECT_AISATU},   //���A
  {msg_union_select_02, (void*)UNION_MSG_MENU_SELECT_OEKAKI},   //���G����
  {msg_union_select_03, (void*)UNION_MSG_MENU_SELECT_TAISEN},   //�ΐ�
  {msg_union_select_04, (void*)UNION_MSG_MENU_SELECT_KOUKAN},   //����
  {msg_union_select_06, (void*)UNION_MSG_MENU_SELECT_GURUGURU}, //���邮�����
  {msg_union_select_07, (void*)UNION_MSG_MENU_SELECT_RECORD},   //���R�[�h�R�[�i�[
  {msg_union_select_05, (void*)UNION_MSG_MENU_SELECT_CANCEL},   //��߂�
};

///���j���[�w�b�_�[(���C�����j���[�p)
static const FLDMENUFUNC_HEADER MenuHeader_MainMenu =
{
	7,		//���X�g���ڐ�
	7,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_NON,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	16,		//�����T�C�YY(�h�b�g
	32-14,		//�\�����WX �L�����P��
	1,		//�\�����WY �L�����P��
	14,		//�\���T�C�YX �L�����P��
	14,		//�\���T�C�YY �L�����P��
};
//SDK_COMPILER_ASSERT(NELEMS(MainMenuList) == MenuHeader_MainMenu.line);



//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * ��b�E�B���h�E�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_TalkStream_WindowSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork)
{
  FLDMSGBG *fldmsg_bg;
  
  if(unisys->fld_msgwin_stream == NULL){
    GF_ASSERT(unisys->msgdata == NULL);
    fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
    unisys->msgdata = FLDMSGBG_CreateMSGDATA( fldmsg_bg, NARC_message_union_dat );
    unisys->fld_msgwin_stream = FLDMSGWIN_STREAM_AddTalkWin(fldmsg_bg, unisys->msgdata);
  }
  else{ //���ɃE�B���h�E���\������Ă���ꍇ�̓��b�Z�[�W�G���A�̃N���A���s��
    FLDMSGWIN_STREAM_ClearMessage(unisys->fld_msgwin_stream);
  }
}

//==================================================================
/**
 * ��b�E�B���h�E�F�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_TalkStream_WindowDel(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fld_msgwin_stream != NULL){
    FLDMSGWIN_STREAM_ClearWindow(unisys->fld_msgwin_stream);
    FLDMSGWIN_STREAM_Delete(unisys->fld_msgwin_stream);
    unisys->fld_msgwin_stream = NULL;
  }

  if(unisys->msgdata != NULL){
    FLDMSGBG_DeleteMSGDATA(unisys->msgdata);
    unisys->msgdata = NULL;
  }
}

//==================================================================
/**
 * ��b�E�B���h�E�F�o�͊J�n
 *
 * @param   unisys		
 * @param   str_id		���b�Z�[�WID
 */
//==================================================================
void UnionMsg_TalkStream_Print(UNION_SYSTEM_PTR unisys, u32 str_id)
{
  FLDMSGPRINT *msg_print;
  
  GF_ASSERT(unisys->fld_msgwin_stream != NULL);
  FLDMSGWIN_STREAM_PrintStart(unisys->fld_msgwin_stream, 0, 0, str_id);
}

//==================================================================
/**
 * ��b�E�B���h�E�F�I���`�F�b�N
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:�I���B�@FALSE:�p����
 */
//==================================================================
BOOL UnionMsg_TalkStream_Check(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fld_msgwin_stream == NULL){
    return TRUE;
  }
  return FLDMSGWIN_STREAM_Print(unisys->fld_msgwin_stream);
}

//==================================================================
/**
 * ��b�E�B���h�E�F�o�͊J�n�ɉ����A���b�Z�[�W�E�B���h�E�������ꍇ�͐������s��
 *
 * @param   unisys		  
 * @param   fieldWork		
 * @param   str_id		  ���b�Z�[�WID
 */
//==================================================================
void UnionMsg_TalkStream_PrintPack(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork, u32 str_id)
{
  UnionMsg_TalkStream_WindowSetup(unisys, fieldWork);
  UnionMsg_TalkStream_Print(unisys, str_id);
}

//==================================================================
/**
 * �u�͂��v�u�������v�I���E�B���h�E�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_YesNo_Setup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork)
{
  FLDMSGBG *fldmsg_bg;

  if(unisys->fldmenu_yesno_func == NULL){
    fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
    unisys->fldmenu_yesno_func = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
  }
}

//==================================================================
/**
 * �u�͂��v�u�������v�I���E�B���h�E�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_YesNo_Del(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fldmenu_yesno_func != NULL){
    FLDMENUFUNC_DeleteMenu(unisys->fldmenu_yesno_func);
    unisys->fldmenu_yesno_func = NULL;
  }
}

//==================================================================
/**
 * �u�͂��v�u�������v�I��҂�
 *
 * @param   unisys		
 * @param   result		���ʑ����(TRUE:�͂��AFALSE:������)
 *
 * @retval  BOOL		TRUE:�I�������B�@FALSE:�I��
 */
//==================================================================
BOOL UnionMsg_YesNo_SelectLoop(UNION_SYSTEM_PTR unisys, BOOL *result)
{
  FLDMENUFUNC_YESNO yes_no;
  
  GF_ASSERT(unisys->fldmenu_yesno_func != NULL);
  
  yes_no = FLDMENUFUNC_ProcYesNoMenu(unisys->fldmenu_yesno_func);
  if(yes_no == FLDMENUFUNC_YESNO_NULL){
    return FALSE;
  }
  else if(yes_no == FLDMENUFUNC_YESNO_YES){
    *result = TRUE;
  }
  else{
    *result = FALSE;
  }
  return TRUE;
}

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
static void UnionMsg_Menu_WindowSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork, const FLDMENUFUNC_LIST *menulist, int list_max, const FLDMENUFUNC_HEADER *menuhead)
{
  FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
  
  if(unisys->fldmenu_listdata == NULL){
    GF_ASSERT(unisys->fldmenu_listdata == NULL && unisys->fldmenu_func == NULL);
    unisys->fldmenu_listdata = FLDMENUFUNC_CreateMakeListData(
      menulist, list_max, unisys->msgdata, HEAPID_UNION);
    unisys->fldmenu_func = FLDMENUFUNC_AddMenu(fldmsg_bg, menuhead, unisys->fldmenu_listdata);
  }
}

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�폜
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void UnionMsg_Menu_WindowDel(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fldmenu_func != NULL){
    FLDMENUFUNC_DeleteMenu(unisys->fldmenu_func);
    unisys->fldmenu_func = NULL;
  }

  if(unisys->fldmenu_listdata != NULL){
    FLDMENUFUNC_DeleteListData(unisys->fldmenu_listdata);
    unisys->fldmenu_listdata = NULL;
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
static u32 UnionMsg_Menu_SelectLoop(UNION_SYSTEM_PTR unisys)
{
  GF_ASSERT(unisys->fldmenu_func != NULL);
  return FLDMENUFUNC_ProcMenu(unisys->fldmenu_func);
}

//==================================================================
/**
 * ���C�����j���[�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_Menu_MainMenuSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork)
{
  UnionMsg_Menu_WindowSetup(unisys, fieldWork, 
    MainMenuList, NELEMS(MainMenuList), &MenuHeader_MainMenu);
}

//==================================================================
/**
 * ���C�����j���[�F�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_Menu_MainMenuDel(UNION_SYSTEM_PTR unisys)
{
  UnionMsg_Menu_WindowDel(unisys);
}

//==================================================================
/**
 * ���C�����j���[�F�I��҂�
 *
 * @param   unisys		
 * 
 * @retval  u32		����
 */
//==================================================================
u32 UnionMsg_Menu_MainMenuSelectLoop(UNION_SYSTEM_PTR unisys)
{
  return UnionMsg_Menu_SelectLoop(unisys);
}

