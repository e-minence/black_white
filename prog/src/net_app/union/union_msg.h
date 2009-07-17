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
///UnionMsg_Menu_SelectLoop�֐��̖߂�l   ��MenuMemberMax�e�[�u���ƕ��т𓯂��ɂ��Ă������ƁI
enum{
  UNION_MSG_MENU_SELECT_AISATU,       ///<���A
  UNION_MSG_MENU_SELECT_OEKAKI,       ///<���G����
  UNION_MSG_MENU_SELECT_BATTLE_2VS2_SINGLE_50,        ///<�ΐ�:2VS2:�V���O��:LV50
  UNION_MSG_MENU_SELECT_BATTLE_2VS2_SINGLE_FREE,      ///<�ΐ�:2VS2:�V���O��:�����Ȃ�
  UNION_MSG_MENU_SELECT_BATTLE_2VS2_SINGLE_STANDARD,  ///<�ΐ�:2VS2:�V���O��:�X�^���_�[�h
  UNION_MSG_MENU_SELECT_KOUKAN,       ///<����
  UNION_MSG_MENU_SELECT_GURUGURU,     ///<���邮�����
  UNION_MSG_MENU_SELECT_RECORD,       ///<���R�[�h�R�[�i�[
  UNION_MSG_MENU_SELECT_CANCEL,       ///<��߂�
  
  UNION_MSG_MENU_SELECT_MAX,    //���ȉ��A���M����鎖�͂Ȃ��ԍ�(���[�J�����j���[�̔���ł̂ݎg�p)
  UNION_MSG_MENU_SELECT_NO_SEND_BATTLE,       ///<�ΐ�(���ꂪ���M����鎖�͂Ȃ�)
  UNION_MSG_MENU_SELECT_NULL = 0xff,  ///<�����I������Ă��Ȃ�(�����l�Ƃ��Ďg�p)
};


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern int UnionMsg_GetMemberMax(int menu_index);
extern void UnionMsg_AllDel(UNION_SYSTEM_PTR unisys);

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

extern void UnionMsg_Menu_BattleMenuSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork, int menu_index);
extern void UnionMsg_Menu_BattleMenuDel(UNION_SYSTEM_PTR unisys);
extern u32 UnionMsg_Menu_BattleMenuSelectLoop(UNION_SYSTEM_PTR unisys, BOOL *next_sub_menu);
