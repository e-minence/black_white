//==============================================================================
/**
 * @file    comm_entry_menu_comm_command.c
 * @brief   �Q����W���j���[����F�ʐM�e�[�u��
 * @author  matsuda
 * @date    2010.01.11(��)
 */
//==============================================================================
#pragma once

#include <gflib.h>
#include "net/network_define.h"


//==============================================================================
//  �萔��`
//==============================================================================

///���j�I����M�R�}���h�ԍ��@�@��CommEntryMenu_CommPacketTbl�ƕ��т𓯂��ɂ��Ă������ƁI�I
enum COLOSSEUM_CMD{
  ENTRYMENU_CMD_START = GFL_NET_CMD_COMM_ENTRY_MENU,
  
  ENTRYMENU_CMD_ENTRY = ENTRYMENU_CMD_START,      ///<�e�փG���g���[��ʒm
  ENTRYMENU_CMD_ENTRY_OK,                         ///<�G���g���[OK
  ENTRYMENU_CMD_ENTRY_NG,                         ///<�G���g���[NG
  ENTRYMENU_CMD_GAME_START_READY,                 ///<�Q�[���J�n�O�̗��E�֎~��ʒB
  ENTRYMENU_CMD_GAME_START_READY_OK,              ///<�Q�[���J�n�O�̏�������
  ENTRYMENU_CMD_GAME_START,                       ///<�Q�[���J�n��ʒm
  ENTRYMENU_CMD_GAME_CANCEL,                      ///<�Q�[���𒆎~���܂�
  ENTRYMENU_CMD_MEMBER_INFO,                      ///<���݂̎Q���҃��X�g

  ENTRYMENU_CMD_MAX,
  ENTRYMENU_CMD_NUM = ENTRYMENU_CMD_MAX - ENTRYMENU_CMD_START,
};


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void CommEntryMenu_AddCommandTable(COMM_ENTRY_MENU_PTR em);
extern void CommEntryMenu_DelCommandTable(void);

extern BOOL CemSend_Entry(const MYSTATUS *myst, BOOL force_entry, BOOL comm_mp);
extern BOOL CemSend_EntryOK(NetID send_id, BOOL comm_mp, const MYSTATUS *myst);
extern BOOL CemSend_EntryNG(NetID send_id, BOOL comm_mp, const MYSTATUS *myst);
extern BOOL CemSend_GameStartReady(BOOL comm_mp);
extern BOOL CemSend_GameStartReadyOK(BOOL comm_mp);
extern BOOL CemSend_GameStart(BOOL comm_mp);
extern BOOL CemSend_GameCancel(BOOL comm_mp);
extern BOOL CemSend_MemberInfo(const ENTRYMENU_MEMBER_INFO *member_info, u8 send_bit, BOOL comm_mp);
