//==============================================================================
/**
 * @file    comm_entry_menu.h
 * @brief   �Q����W���j���[����̃w�b�_
 * @author  matsuda
 * @date    2009.07.28(��)
 */
//==============================================================================
#pragma once

#include "savedata/mystatus.h"
#include "savedata/mystatus_local.h"
#include "field/fieldmap_proc.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�ő�Q���l��
#define COMM_ENTRY_USER_MAX     (4)

///�ڑ����[�h
typedef enum{
  ///�e�@
  COMM_ENTRY_MODE_PARENT,
  ///�q�@(�e�@�I��)
  COMM_ENTRY_MODE_CHILD,
  ///�q�@(���ɐe�ƃl�S�V�G�[�V�������݂Őڑ��ς�) ���R���V�A���ŏ��̃����o�[
  COMM_ENTRY_MODE_CHILD_PARENT_CONNECTED,   
  ///�q�@(�ڑ�����e�@���O������w��) ���R���V�A������
  COMM_ENTRY_MODE_CHILD_PARENT_DESIGNATE,
}COMM_ENTRY_MODE;

///�J�Â���Q�[���̎��
typedef enum{
  COMM_ENTRY_GAMETYPE_COLOSSEUM,    ///<�R���V�A��
  COMM_ENTRY_GAMETYPE_MUSICAL,      ///<�~���[�W�J��
  COMM_ENTRY_GAMETYPE_SUBWAY,       ///<�n���S
  
  COMM_ENTRY_GAMETYPE_MAX,
}COMM_ENTRY_GAMETYPE;


//==============================================================================
//  �^��`
//==============================================================================
typedef struct _COMM_ENTRY_MENU_SYSTEM * COMM_ENTRY_MENU_PTR;
typedef struct _ENTRYMENU_MEMBER_INFO ENTRYMENU_MEMBER_INFO;

///�󂯓��ꌋ��
typedef enum{
  COMM_ENTRY_ANSWER_NULL,
  COMM_ENTRY_ANSWER_COMPLETION,   ///<�󂯓���OK
  COMM_ENTRY_ANSWER_REFUSE,       ///<�󂯓���NG
}COMM_ENTRY_ANSWER;

///����
typedef enum{
  COMM_ENTRY_RESULT_NULL,         ///<�������s��
  COMM_ENTRY_RESULT_SUCCESS,      ///<�����o�[���W�܂���
  COMM_ENTRY_RESULT_CANCEL,       ///<�L�����Z�����ďI��
  COMM_ENTRY_RESULT_ERROR,        ///<�G���[�ŏI��
}COMM_ENTRY_RESULT;

///�q�@���G���g���[�������A�e�@����̕Ԏ�
typedef enum{
  ENTRY_PARENT_ANSWER_NULL,             ///<�Ԏ�����M
  ENTRY_PARENT_ANSWER_OK,               ///<�G���g���[OK
  ENTRY_PARENT_ANSWER_NG,               ///<�G���g���[NG
}ENTRY_PARENT_ANSWER;


//==============================================================================
//  �\���̒�`
//==============================================================================
///�G���g���[���j���[���g�p���ĎQ������ׂ̃r�[�R�����
typedef struct{
  MYSTATUS mystatus;    ///<���g��MyStatus
  u8 mac_address[6];    ///<���g��MacAddress
  u8 padding[2];
}COMM_ENTRY_BEACON;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern COMM_ENTRY_MENU_PTR CommEntryMenu_Setup(const MYSTATUS *myst, FIELDMAP_WORK *fieldWork, int min_num, int max_num, HEAPID heap_id, COMM_ENTRY_MODE entry_mode, COMM_ENTRY_GAMETYPE game_type, const u8 *parent_mac_address);
extern void CommEntryMenu_Exit(COMM_ENTRY_MENU_PTR em);
extern void CommEntryMenu_Entry(COMM_ENTRY_MENU_PTR em, int netID, const MYSTATUS *myst, BOOL force_entry, const u8 *mac_address);
extern COMM_ENTRY_RESULT CommEntryMenu_Update(COMM_ENTRY_MENU_PTR em);
extern void CommEntryMenu_SetGameStart(COMM_ENTRY_MENU_PTR em);
extern void CommEntryMenu_SetGameCancel(COMM_ENTRY_MENU_PTR em);
extern COMM_ENTRY_ANSWER CommEntryMenu_GetAnswer(COMM_ENTRY_MENU_PTR em, int netID, BOOL exit_mode);
extern void CommEntryMenu_SetSendFlag(COMM_ENTRY_MENU_PTR em, int netID);
extern int CommEntryMenu_GetCompletionNum(COMM_ENTRY_MENU_PTR em);
extern void CommEntyrMenu_MemberInfoReserveUpdate(COMM_ENTRY_MENU_PTR em);
extern ENTRYMENU_MEMBER_INFO * CommEntryMenu_GetMemberInfo(COMM_ENTRY_MENU_PTR em);
extern void CommEntryMenu_RecvMemberInfo(COMM_ENTRY_MENU_PTR em, const ENTRYMENU_MEMBER_INFO *member_info);
extern void CommEntryMenu_SetEntryAnswer(COMM_ENTRY_MENU_PTR em, ENTRY_PARENT_ANSWER answer, const MYSTATUS *myst);
extern u32 CommEntryMenu_GetMemberInfoSize(void);
