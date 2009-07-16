//==============================================================================
/**
 * @file    union_msg.h
 * @brief   ���j�I�����[���F�c�[����(�I�[�o�[���C�̈�ɔz�u����܂�)
 * @author  matsuda
 * @date    2009.07.13(��)
 */
//==============================================================================
#pragma once

//==============================================================================
//  �萔��`
//==============================================================================
///UnionMsg_Menu_SelectLoop�֐��̖߂�l
enum{
  UNION_MSG_MENU_SELECT_AISATU,       ///<���A
  UNION_MSG_MENU_SELECT_OEKAKI,       ///<���G����
  UNION_MSG_MENU_SELECT_TAISEN,       ///<�ΐ�
  UNION_MSG_MENU_SELECT_KOUKAN,       ///<����
  UNION_MSG_MENU_SELECT_GURUGURU,     ///<���邮�����
  UNION_MSG_MENU_SELECT_RECORD,       ///<���R�[�h�R�[�i�[
  UNION_MSG_MENU_SELECT_CANCEL,       ///<��߂�
  
  UNION_MSG_MENU_SELECT_MAX,
  UNION_MSG_MENU_SELECT_NULL = 0xff,  ///<�����I������Ă��Ȃ�(�����l�Ƃ��Ďg�p)
};


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void UnionMsg_TalkStream_WindowSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork);
extern void UnionMsg_TalkStream_WindowDel(UNION_SYSTEM_PTR unisys);
extern void UnionMsg_TalkStream_Print(UNION_SYSTEM_PTR unisys, u32 str_id);
extern BOOL UnionMsg_TalkStream_Check(UNION_SYSTEM_PTR unisys);
extern void UnionMsg_TalkStream_PrintPack(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork, u32 str_id);

extern void UnionMsg_YesNo_Setup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork);
extern void UnionMsg_YesNo_Del(UNION_SYSTEM_PTR unisys);
extern BOOL UnionMsg_YesNo_SelectLoop(UNION_SYSTEM_PTR unisys, BOOL *result);

extern void UnionMsg_Menu_MainMenuSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork);
extern void UnionMsg_Menu_MainMenuDel(UNION_SYSTEM_PTR unisys);
extern u32 UnionMsg_Menu_MainMenuSelectLoop(UNION_SYSTEM_PTR unisys);

