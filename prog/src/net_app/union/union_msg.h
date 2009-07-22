//==============================================================================
/**
 * @file    union_msg.h
 * @brief   ���j�I�����[���F�c�[����(�I�[�o�[���C�̈�ɔz�u����܂�)
 * @author  matsuda
 * @date    2009.07.13(��)
 */
//==============================================================================
#pragma once

#include "union_types.h"

//==============================================================================
//  �萔��`
//==============================================================================
///UnionMsg_Menu_SelectLoop�֐��̖߂�l(PLAY_CATEGORY�ȊO�̎w��)
enum{
  UNION_MENU_SELECT_CANCEL = UNION_PLAY_CATEGORY_UNION,///<�L�����Z��(���j�I���ɖ߂�Ƃ����Ӗ�)
  
  //���ȉ��A���M����鎖�͂Ȃ��ԍ�(���[�J�����j���[�̔���ł̂ݎg�p)
  //  ���[�J���ł����g�p���Ȃ��̂ŏ����̔ԍ��Ɣ���Ă����v
  UNION_MENU_NO_SEND_BATTLE = UNION_PLAY_CATEGORY_MAX,       ///<�ΐ�(���ꂪ���M����鎖�͂Ȃ�)
  UNION_MENU_SELECT_NULL = 0xff,  ///<�����I������Ă��Ȃ�(�����l�Ƃ��Ďg�p)
};


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern int UnionMsg_GetMemberMax(UNION_PLAY_CATEGORY menu_index);
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
