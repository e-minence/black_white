//==============================================================================
/**
 * @file    union_comm_command.h
 * @brief   ���j�I���F�ʐM�R�}���h
 * @author  matsuda
 * @date    2009.07.14(��)
 */
//==============================================================================
#pragma once

#include <gflib.h>
#include "net/network_define.h"
#include "colosseum_types.h"


//==============================================================================
//  �萔��`
//==============================================================================

///���j�I����M�R�}���h�ԍ��@�@��Union_CommPacketTbl�ƕ��т𓯂��ɂ��Ă������ƁI�I
enum UNION_CMD{
  UNION_CMD_START = GFL_NET_CMD_UNION,
  
  UNION_CMD_SHUTDOWN = UNION_CMD_START,     ///<�ؒf
  UNION_CMD_MAINMENU_LIST_RESULT,           ///<���C�����j���[�I������
  UNION_CMD_MAINMENU_LIST_RESULT_ANSWER,    ///<���C�����j���[�I�����ʂ̕Ԏ�
  UNION_CMD_MYSTATUS,                       ///<MYSTATUS
  UNION_CMD_TRAINERCARD_PARAM,              ///<�g���[�i�[�J�[�h���
  UNION_CMD_COLOSSEUM_ENTRY,                ///<�R���V�A���G���g���[
  UNION_CMD_COLOSSEUM_ENTRY_ANSWER,         ///<�R���V�A���G���g���[����
  UNION_CMD_COLOSSEUM_ENTRY_ALL_READY,      ///<�R���V�A���G���g���[�S��������
  
  UNION_CMD_MAX,
  UNION_CMD_NUM = UNION_CMD_MAX - UNION_CMD_START,
};


//==============================================================================
//  �O���f�[�^
//==============================================================================
extern const NetRecvFuncTable Union_CommPacketTbl[];


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern BOOL UnionSend_MainMenuListResult(u32 select_list);
extern BOOL UnionSend_MainMenuListResultAnswer(BOOL yes_no);
extern BOOL UnionSend_Mystatus(UNION_SYSTEM_PTR unisys);
extern BOOL UnionSend_TrainerCardParam(UNION_SYSTEM_PTR unisys);
extern BOOL UnionSend_ColosseumEntryStatus(COLOSSEUM_BASIC_STATUS *basic_status);
extern BOOL UnionSend_ColosseumEntryAnswer(int send_netid, COMM_ENTRY_ANSWER answer);
extern BOOL UnionSend_ColosseumEntryAllReady(void);
